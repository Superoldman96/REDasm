#include "goto.h"

GotoDialog::GotoDialog(RDContext* ctx, QWidget* parent)
    : QDialog{parent}, m_ui{this}, m_context{ctx} {

    m_symbolsmodel = new SymbolsModel(ctx, true, 0, m_ui.tvsymbols);
    m_symbolsmodel->set_highlight_symbol(true);

    m_filtermodel = new QSortFilterProxyModel(m_ui.tvsymbols);
    m_filtermodel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_filtermodel->setFilterKeyColumn(-1);
    m_filtermodel->setSourceModel(m_symbolsmodel);
    m_ui.tvsymbols->setModel(m_filtermodel);

    m_ui.tvsymbols->header()->setStretchLastSection(false);
    m_ui.tvsymbols->header()->setSectionResizeMode(2, QHeaderView::Stretch);

    connect(m_ui.lesearch, &QLineEdit::textChanged, this,
            [=](const QString&) { this->validate_and_filter_entry(); });
    connect(m_ui.lesearch, &QLineEdit::returnPressed, this,
            &GotoDialog::on_return_pressed);
    connect(m_ui.tvsymbols, &QTreeView::doubleClicked, this,
            &GotoDialog::on_item_selected);
    connect(m_ui.tvsymbols, &QTreeView::doubleClicked, this,
            &GotoDialog::accept);
    connect(m_ui.rbaddress, &QRadioButton::clicked, this,
            &GotoDialog::on_mode_changed);
    connect(m_ui.rboffset, &QRadioButton::clicked, this,
            &GotoDialog::on_mode_changed);

    this->on_mode_changed(); // initialize placeholder
}

bool GotoDialog::check_valid_address() {
    if(m_ui.lesearch->text().simplified().isEmpty()) return false;

    if(m_filtermodel->rowCount() == 1) {
        QModelIndex srcindex =
            m_filtermodel->mapToSource(m_filtermodel->index(0, 0));

        if(srcindex.isValid()) {
            this->address = m_symbolsmodel->address(srcindex);
            m_validaddress = true;
        }
    }

    return m_validaddress && rd_is_address(m_context, this->address);
}

GotoDialog::AddressingMode GotoDialog::addressing_mode() const {
    if(m_ui.rboffset->isChecked()) return GotoDialog::AddressingMode::OFFSET;
    return GotoDialog::AddressingMode::ADDRESS;
}

void GotoDialog::validate_and_filter_entry() {
    QPushButton* okbutton = m_ui.buttonbox->button(QDialogButtonBox::Ok);
    QString s = m_ui.lesearch->text();

    if(s.simplified().isEmpty()) {
        m_validaddress = false;

        okbutton->setEnabled(false);
        m_filtermodel->setFilterFixedString({});
        return;
    }

    bool ok = false;

    if(this->addressing_mode() == AddressingMode::OFFSET) {
        RDOffset offset = s.toULongLong(&ok, 16);

        if(ok && rd_is_offset(m_context, offset))
            m_validaddress = rd_to_address(m_context, offset, &this->address);
        else
            m_validaddress = false;
    }
    else {
        this->address = s.toULongLong(&ok, 16);

        m_validaddress =
            ok ? rd_is_address(m_context, this->address)
               : rd_get_address(m_context, qUtf8Printable(s), &this->address);

        m_filtermodel->setFilterFixedString(s);
    }

    okbutton->setEnabled(m_validaddress);
}

void GotoDialog::on_item_selected(const QModelIndex& index) {
    QModelIndex srcindex = m_filtermodel->mapToSource(index);
    if(!srcindex.isValid()) return;

    this->address = m_symbolsmodel->address(srcindex);
    this->accept();
}

void GotoDialog::on_return_pressed() {
    if(this->check_valid_address()) this->accept();
}

void GotoDialog::on_mode_changed() {         // NOLINT
    m_filtermodel->setFilterFixedString({}); // reset filter on mode change

    if(this->addressing_mode() == AddressingMode::OFFSET) {
        m_ui.lesearch->setPlaceholderText("Offset");
        m_ui.tvsymbols->setEnabled(false);
    }
    else {
        m_ui.lesearch->setPlaceholderText("Address or Symbol");
        m_ui.tvsymbols->setEnabled(true);
    }

    this->validate_and_filter_entry(); // repeat filtering on input
    QPushButton* okbutton = m_ui.buttonbox->button(QDialogButtonBox::Ok);
    okbutton->setEnabled(this->check_valid_address());
}
