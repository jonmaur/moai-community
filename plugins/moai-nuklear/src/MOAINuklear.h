#ifndef	MOAINUKLEAR_H
#define	MOAINUKLEAR_H
#include <moai-core/headers.h>
#include <moai-sim/MOAIRenderable.h>

//================================================================//
// MOAINuklear
//================================================================//
/**	@lua	MOAINuklear
@text	Renders Nuklear based GUI
*/
class MOAINuklear :
	public MOAIRenderable {
public:

	//----------------------------------------------------------------//
	MOAINuklear();
	virtual				~MOAINuklear();
	void		Render();
};

#endif

