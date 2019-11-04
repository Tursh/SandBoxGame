//
// Created by musique88 on 11/4/19.
//

#include <Inventory/Inventory.h>

unsigned int Inventory::addItem(Item newItem, unsigned int quantity)
{
    return 0;
}

const Item Inventory::getItem(int index)
{
    return Item();
}

const Inventory::Slot &Inventory::getItemSlot(int index)
{
    return items_[index];
}

Inventory::Slot Inventory::removeItem(int index, int quantity)
{
    Slot temp = items_[index];
    items_.erase(items_.begin() + index);
    return temp;
}

void Inventory::swapItemWithMouseItem(int index)
{
    Slot temp = mouseSlot_;
    mouseSlot_ = items_[index];
    items_[index] = temp;
}

Inventory::Slot Inventory::getItemFromMouseItem()
{
    Slot temp = mouseSlot_;
    mouseSlot_ = {0, nullptr};
    return temp;
}
