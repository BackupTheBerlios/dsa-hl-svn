#include "ListboxWrappedTextItem.h"

#include "CEGUIImage.h"

namespace CEGUI {

ListboxWrappedTextItem::ListboxWrappedTextItem(const String& text, uint item_id, void* item_data, bool disabled, bool auto_delete)
: ListboxTextItem(text, item_id, item_data, disabled, auto_delete)
{
	d_textFormatting = LeftAligned;
}

ListboxWrappedTextItem::~ListboxWrappedTextItem()
{
}

void ListboxWrappedTextItem::setTextFormatting(TextFormatting fmt)
{
	d_textFormatting = fmt;
}

const TextFormatting& ListboxWrappedTextItem::getTextFormatting() const
{
	return d_textFormatting;
}

void ListboxWrappedTextItem::draw(const Vector3& position, float alpha, const Rect& clipper) const
{
	if (d_selected && (d_selectBrush != NULL))
    {
        d_selectBrush->draw(clipper, position.d_z, clipper, getModulateAlphaColourRect(d_selectCols, alpha));
    }
 
    const Font* fnt = getFont();

    if (fnt != NULL)
    {
         fnt->drawText(d_itemText, clipper, position.d_z, clipper, d_textFormatting, getModulateAlphaColourRect(d_textCols, alpha));
    }
}

Size ListboxWrappedTextItem::getPixelSize() const
{
	 Size size = ListboxTextItem::getPixelSize();

     const Font* fnt = getFont();
 
     if (fnt != NULL)
     {		 
		 Rect formatRect;

		 formatRect.setPosition(Point(0,0));
		 formatRect.setSize(size);
		 uint lines = fnt->getFormattedLineCount(d_itemText, formatRect, d_textFormatting);
         size.d_height *= lines;
     }

	 return size;
}


}
