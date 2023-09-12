#ifndef __RCNAMESPACE_H__
#define __RCNAMESPACE_H__

#include <qicon.h>
#include <qpixmap.h>
#include <qvariant.h>

namespace Rcspace{
	///Icon ö������,���ֻ֧��16��
	enum RcIconEnum
	{
		riskAbnormal = 0,	//����-�쳣
		riskOverLoss,		//����-����
		riskForceClose,		//����-ǿƽ
		riskMarginCall,		//����-׷��
		riskWarning,		//����-��ʾ
		riskNormal,			//����-����
		noIcon,				//no-Icon
	};
	///TextAlignment ö�����ͣ����ֻ֧��16��
	enum RcTextAlignment
	{
		defaultTextAlign = 0,	//no specialized
		textAlignLeftTop,		//left-top
		textAlignLeftCenter,	//left-center
		textAlignLeftBottom,	//left-bottom
		textAlignCenterTop,		//center-top
		textAlignCenterCenter,	//center-center
		textAlignCenterBottom,	//center-center
		textAlignRightTop,		//right-top
		textAlignRightCenter,	//right-center
		textAlignRightBottom,	//right-bottom
	};

	QIcon specialIcon(RcIconEnum iconEnum);
	QVariant specialTextAlignment(RcTextAlignment alignEnum);
}

#endif