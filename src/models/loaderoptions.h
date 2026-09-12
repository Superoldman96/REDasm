#pragma once

#include <QAbstractListModel>
#include <redasm/redasm.h>

class LoaderOptionsModel: public QAbstractItemModel {
    Q_OBJECT

    // a top-level node: either a named group, or the ungrouped bucket
    struct Node {
        QString name;      // empty for the ungrouped bucket
        QVector<int> opts; // indices into m_options
    };

public:
    explicit LoaderOptionsModel(QObject* parent = nullptr);
    void set_test_result(RDTestResult* tr);

public Q_SLOTS:
    void reset_default();

public:
    [[nodiscard]] QModelIndex index(int row, int col,
                                    const QModelIndex& parent) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex& index) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index,
                                int role) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                      int role) const override;
    [[nodiscard]] int columnCount(const QModelIndex&) const override;
    [[nodiscard]] int rowCount(const QModelIndex& parent) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value,
                 int role) override;

private:
    [[nodiscard]] const RDLoaderOption* option_at(const QModelIndex&) const;

private:
    RDTestResult* m_testresult{nullptr};
    RDLoaderOptionSlice m_options{};
    QVector<Node> m_nodes;
    bool m_grouped{false};
};
