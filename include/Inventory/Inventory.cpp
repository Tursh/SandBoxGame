//
// Created by musique88 on 11/4/19.
//

#include "Inventory.h"
#include <algorithm>

bool Inventory::addItem(Item *newItem)
{
	if(volume_ + newItem->volume_ <= maxVolume_ && weight_ + newItem->weight_ <= maxWeight_)
	{
		auto it = std::find(items_.begin(), items_.end(), newItem);

		return true;
	}
	return false;
}

const Item *Inventory::getItem(int index)
{
	return items_[index].second;
}


Inventory::Slot Inventory::removeItem(int index, int quantity)
{
    Slot slot = items_[index];
    if(slot.first < quantity)
    {
        slot.first -= quantity;
        return std::make_pair(quantity, slot.second);
    }
    else
    {
        items_.erase(items_.begin() + index);
        return slot;
    }
}


void Inventory::swapItemWithMouseItem(int index)
{
    Slot temp = mouseItem_;
    mouseItem_ = items_[index];
    items_[index] = temp;
}

Inventory::Slot Inventory::getItemFromMouseItem()
{
    Slot temp = mouseItem_;
    mouseItem_ = {0, nullptr};
	return temp;
}