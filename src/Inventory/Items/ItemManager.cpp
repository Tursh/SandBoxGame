/*
 * ItemManager.cpp
 *
 * Created by tursh on 11/4/19.
*/

#include <Utils/Log.h>
#include "Inventory/Items/ItemManager.h"

namespace ItemManager
{
    const char *ITEM_INFO_PATH = "res/items.org";

    std::map<unsigned int, std::weak_ptr<ItemObject>> loadedItems;

    FILE *file;

    void init()
    {
        file = fopen(ITEM_INFO_PATH, "r");
    }

    Item loadItem(unsigned int ID)
    {
#ifndef NDEBUG
        if (file == nullptr)
        logError("Item manager was not initialized!");
#endif
        //TODO load item for item info file
    }

    Item getItem(unsigned int ID)
    {
        auto it = loadedItems.find(ID);

        if (it != loadedItems.end())
        {
            if (!it->second.expired())
                return it->second.lock();
            else
                loadedItems.erase(it);
        }

        return loadItem(ID);
    }

    void terminate()
    {
        fclose(file);
    }
}