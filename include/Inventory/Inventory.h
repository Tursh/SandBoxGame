//
// Created by musique88 on 11/4/19.
//

#pragma once

#include <vector>
#include <Inventory/Items/Item.h>

class Inventory
{
	float maxWeight_;

	float maxVolume_;

	float weight_;

	float volume_;

	std::vector<std::tuple<unsigned int, Item*>> items_;

	Item *mouseItem_;

public:
	/**
	 * Add item to inventory
	 * @param newItem Item to add
	 * @return Item added succesfully (not full)
	 */
	bool addItem(Item *newItem);

	/**
	 * Return item at index
	 * @param index Index of item
	 * @return item at index
	 */
	const Item  *getItem(int index);

	/**
	 * Clear item in index and returns it
	 * @param index Index of item to clear
	 * @return Item at index
	 */
	Item *removeItem(int index);

	/**
	 * Swap item in index with mouseItem
	 * @param index Index of item
	 */
	void swapItemWithMouseItem(int index);

	/**
	 * Return item from mouse item and reset mouse item
	 * @return item stored in mouseItem
	 */
	Item *getItemFromMouseItem();

};
