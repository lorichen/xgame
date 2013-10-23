#include "TxGuiReResourceProvider.h"
#include "stringhelper\StringConv.h"

namespace xs{

void TxGuiResourceProvider::loadRawDataContainer(const xsgui::String& filename, xsgui::RawDataContainer& output, const xsgui::String& resourceGroup)
{
	char *pChar = xsgui::StringConv::Utf8ToChar((char*)filename.c_str());	
	CStreamHelper pStream = getFileSystem()->open(pChar);
	output.setSize(0);
	output.setData(0);
	if(!pStream.isNull())
	{
		uint size = pStream->getLength();
		uchar *pData = new uchar[size+1];
		pData[size] = 0;
		if(pData)
		{
			pStream->read(pData,size);
			//xsgui::makeMap(pData,size,'LAND');
			output.setData(pData);
			output.setSize(size);
		}
	}
	xsgui::StringConv::DeletePtr(pChar);
}
void TxGuiResourceProvider::unloadRawDataContainer(xsgui::RawDataContainer& data)
{
	if(data.getDataPtr())	
	{
		delete[] data.getDataPtr();
		data.setData(0);
	}
}
}