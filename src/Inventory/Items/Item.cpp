//
// Created by musique88 on 11/4/19.
//

#include "Inventory/Items/Item.h"
#include "Inventory/Items/ItemManager.h"

const char *ItemObject::getName()
{
    return defaultName_;
}

const char *ItemObject::getDescription()
{
    return defaultDescription_;
}

ItemObject::ItemObject(unsigned int ID, ItemType type, const char *defaultName, const char *defaultDescription,
                       float weight,
                       float volume)
        : ID_(ID), type_(type), defaultName_(defaultName), defaultDescription_(defaultDescription), weight_(weight),
          volume_(volume) {}

ItemObject::ItemObject(Item item, float weight, float volume)
        : ID_(item->ID_), type_(item->type_), defaultName_(item->defaultName_),
          defaultDescription_(item->defaultDescription_), weight_(weight),
          volume_(volume) {}

Item ItemObject::getItem(unsigned int ID)
{
    return ItemManager::getItem(ID);
}

Item ItemObject::getAlternativeItem(Item baseItem, float weight, float volume)
{
    return std::make_shared<ItemObject>(baseItem, weight, volume);
}
