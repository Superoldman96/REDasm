#pragma once

#include <QButtonGroup>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QTreeView>
#include <QVBoxLayout>

namespace ui {

struct GotoDialog {
    QLineEdit* lesearch;
    QTreeView* tvsymbols;
    QRadioButton *rbaddress, *rboffset;
    QDialogButtonBox* buttonbox;

    explicit GotoDialog(QDialog* self) {
        self->setAttribute(Qt::WA_DeleteOnClose);
        self->setWindowTitle("Goto");
        self->resize(640, 330);
        self->setModal(true);

        this->lesearch = new QLineEdit();

        this->tvsymbols = new QTreeView();
        this->tvsymbols->header()->setStretchLastSection(true);
        this->tvsymbols->setUniformRowHeights(true);
        this->tvsymbols->setRootIsDecorated(false);

        auto* vbox = new QVBoxLayout(self);
        vbox->addWidget(this->lesearch);
        vbox->addWidget(this->tvsymbols, 1);

        this->rbaddress = new QRadioButton();
        this->rbaddress->setText("By address");
        this->rbaddress->setChecked(true);

        this->rboffset = new QRadioButton();
        this->rboffset->setText("By offset");

        auto* grpmode = new QButtonGroup(self);
        grpmode->setExclusive(true);
        grpmode->addButton(this->rbaddress);
        grpmode->addButton(this->rboffset);

        this->buttonbox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                               QDialogButtonBox::Cancel);
        QObject::connect(this->buttonbox, &QDialogButtonBox::accepted, self,
                         &QDialog::accept);
        QObject::connect(this->buttonbox, &QDialogButtonBox::rejected, self,
                         &QDialog::reject);

        auto* hbox = new QHBoxLayout();
        hbox->addWidget(this->rbaddress);
        hbox->addWidget(this->rboffset);
        hbox->addStretch();
        hbox->addWidget(this->buttonbox);

        vbox->addLayout(hbox);
    }
};

} // namespace ui
