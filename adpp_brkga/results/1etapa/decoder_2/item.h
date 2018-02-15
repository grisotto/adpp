#ifndef ITEM_H
#define ITEM_H

class Item{
    public:
        int width;
        int height;
        int demand;
        int b;
        int value;
        
        bool operator <(const Item& i) const{
            return (this->value > i.value);
        }
};
#endif
