//
// Created by musique88 on 11/4/19.
//

#pragma once

#include <vector>
#include <Inventory/Items/Item.h>

class Inventory
{
    //*< <quantity, item>
    typedef std::pair<unsigned int, Item> Slot;

	float maxWeight_;

	float maxVolume_;

	float weight_;

	float volume_;

	std::vector<Slot> items_;

	Slot mouseSlot_;

public:
	/**
	 * Add a quantity of item in the inventory
	 * @param newItem Item to add
	 * @param quantity How many item to add
	 * @return The rest of the item that could not be added (inventory full)
	 */
	unsigned int addItem(Item newItem, unsigned int quantity = 1);

	/**
	 * Return item at index
	 * @param index Index of item
	 * @return item at index
	 */
	const Item getItem(int index);

	/**
	 * Return the item slot
	 * @param index item index in inventory
	 * @return Item Slot <quantity and item>
	 */
	const Slot &getItemSlot(int index);

	/**
	 * Clear item at index and returns it
	 * @param index Index of item to clear
	 * @return Item slot at index <quantity and item>
	 */
	Slot removeItem(int index, int quantity = 1);

	/**
	 * Swap item in index with mouseItem
	 * @param index Index of item
	 */
	void swapItemWithMouseItem(int index);

	/**
	 * Return item from mouse item and reset mouse item
	 * @return item stored in mouseItem
	 */
	Slot getItemFromMouseItem();

};
