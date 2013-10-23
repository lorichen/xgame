#ifndef __TxGuiReResourceProvider_H__
#define __TxGuiReResourceProvider_H__

#include "TxGuiReRenderer.h"
#include "TxSysGuiResourceProvider.h"

namespace xs{

class TxGuiRendererAPI TxGuiResourceProvider : public xsgui::ResourceProvider
{
public:
	TxGuiResourceProvider() { }
	~TxGuiResourceProvider() { }

	void  loadRawDataContainer(const xsgui::String& filename, xsgui::RawDataContainer& output, const xsgui::String& resourceGroup);
	void  unloadRawDataContainer(xsgui::RawDataContainer& data);
};

}

#endif // __TxGuiDEFAULTRESOURCEPROVIDER_H__
