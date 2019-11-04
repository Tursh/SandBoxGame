//
// Created by musique88 on 11/4/19.
//
#pragma once

#include <memory>

enum ItemType
{
    RESOURCE, BLOC, TOOL, WEARABLE
};

class ItemObject
{
    const char *defaultName_;

    const char *defaultDescription_;
public:
    typedef std::shared_ptr<ItemObject> Item;

    const unsigned int ID_;

    const ItemType type_;

    const float weight_;

    const float volume_;

    ItemObject(unsigned int ID, ItemType type, const char *defaultName, const char *defaultDescription, float weight,
               float volume);

    ItemObject(Item item, float weight, float volume);

    virtual const char *getName();

    virtual const char *getDescription();

    /**
     * Get unexisting item
     * @param ID Item ID
     * @return new Item
     */
    static Item getItem(unsigned int ID);

    /**
     * Create an Item with modified attributes
     * @param baseItem Base for the new item
     * @param weight new weight
     * @param volume new volume
     * @return New item with modified attributes
     */
    static Item getAlternativeItem(Item baseItem, float weight, float volume);
};

typedef ItemObject::Item Item;