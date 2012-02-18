#include "SurfaceDesc.h"
#include "Construction.h"

DynamicsSurfaceDesc::DynamicsSurfaceDesc()
{
	ContactSlip1   = .0001;
	ContactSlip2   = .0001;
	mu			   = 2.0f;
	mu2			   = 0;
	Bounce		   = 0.02;
	BounceVelocity = 0.02;
	SoftERP        = 0.1f;
	SoftCFM        = 0.01f;
	Owner          = NULL;
	ParentConstruction      = NULL;


}

DynamicsSurfaceDesc::~DynamicsSurfaceDesc()
{
}

void DynamicsSurfaceDesc::Combine (DynamicsSurfaceDesc* d1, DynamicsSurfaceDesc* d2 )
{
	if(d1 == NULL && d2 == NULL )
		return;
	if(d1 != NULL && d2 == NULL )
		*this = *d1;
	else if(d1 == NULL && d2 != NULL )
		*this = *d2;

	else if(d1 != NULL && d2 != NULL )
	{
		ContactSlip1 = MAX( d1->ContactSlip1, d2->ContactSlip1 ) ;
		ContactSlip2 = MAX( d1->ContactSlip2, d2->ContactSlip2 ) ;

		Bounce			= (d1->Bounce + d2->Bounce) * .5;
		BounceVelocity  = (d1->BounceVelocity + d2->BounceVelocity) * .5;


		//favor the more accurate ERP
		SoftERP = MAX( d1->SoftERP, d2->SoftERP );
		SoftCFM = MIN( d1->SoftCFM, d2->SoftCFM );


		mu	= (d1->mu  + d2->mu ) * 0.5;
		mu2	= (d1->mu2 + d2->mu2) * 0.5;
	}
}
