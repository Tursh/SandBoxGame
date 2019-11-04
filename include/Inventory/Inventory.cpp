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
	return items_[index];
}

Item *Inventory::removeItem(int index)
{
	Item *item = items_[index];
	items_

	return nullptr;
}

void Inventory::swapItemWithMouseItem(int index)
{

}

Item *Inventory::getItemFromMouseItem()
{
	return nullptr;
}
