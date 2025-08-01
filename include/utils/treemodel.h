/*
 * Fooyin
 * Copyright © 2023, Luke Taylor <LukeT1@proton.me>
 *
 * Fooyin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fooyin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Fooyin.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <QAbstractItemModel>

namespace Fooyin {
template <class Item>
class TreeModel : public QAbstractItemModel
{
public:
    explicit TreeModel(QObject* parent = nullptr)
        : QAbstractItemModel{parent}
        , m_root{std::make_unique<Item>()}
    { }

    ~TreeModel() override = default;

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex& parent) const override
    {
        if(!hasIndex(row, column, parent)) {
            return {};
        }

        Item* parentItem = itemForIndex(parent);
        Item* childItem  = parentItem->child(row);
        if(childItem) {
            return createIndex(row, column, childItem);
        }
        return {};
    }

    [[nodiscard]] QModelIndex parent(const QModelIndex& index) const override
    {
        if(!index.isValid()) {
            return {};
        }

        auto* childItem  = static_cast<Item*>(index.internalPointer());
        Item* parentItem = childItem->parent();

        if(parentItem == m_root.get()) {
            return {};
        }

        return createIndex(parentItem->row(), 0, parentItem);
    }

    [[nodiscard]] int rowCount(const QModelIndex& parent) const override
    {
        if(parent.column() > 0) {
            return 0;
        }

        Item* parentItem = itemForIndex(parent);
        return parentItem->childCount();
    }

    [[nodiscard]] int columnCount(const QModelIndex& /*parent*/) const override
    {
        return 1;
    }

    [[nodiscard]] QModelIndex indexOfItem(const Item* item) const
    {
        if(item) {
            if(item == rootItem()) {
                return {};
            }
            return createIndex(item->row(), 0, item);
        }
        return {};
    }

    [[nodiscard]] Item* itemForIndex(const QModelIndex& index) const
    {
        if(!index.isValid()) {
            return rootItem();
        }
        return static_cast<Item*>(index.internalPointer());
    }

protected:
    [[nodiscard]] Item* rootItem() const
    {
        return m_root.get();
    }

    virtual void resetRoot()
    {
        m_root = std::make_unique<Item>();
    }

    void invalidateData()
    {
        beginResetModel();
        endResetModel();
    }

private:
    std::unique_ptr<Item> m_root;
};
} // namespace Fooyin
