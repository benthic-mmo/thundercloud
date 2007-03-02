/** 
 * @file llviewerjointmesh.h
 * @brief Implementation of LLViewerJointMesh class
 *
 * Copyright (c) 2001-$CurrentYear$, Linden Research, Inc.
 * $License$
 */

#ifndef LL_LLVIEWERJOINTMESH_H
#define LL_LLVIEWERJOINTMESH_H

#include "llviewerjoint.h"
#include "llviewerimage.h"
#include "llpolymesh.h"
#include "v4color.h"
#include "llapr.h"

class LLDrawable;
class LLFace;
class LLCharacter;
class LLTexLayerSet;

typedef enum e_avatar_render_pass
{
	AVATAR_RENDER_PASS_SINGLE,
	AVATAR_RENDER_PASS_CLOTHING_INNER,
	AVATAR_RENDER_PASS_CLOTHING_OUTER
} EAvatarRenderPass;

class LLSkinJoint
{
public:
	LLSkinJoint();
	~LLSkinJoint();
	BOOL setupSkinJoint( LLViewerJoint *joint);

	LLViewerJoint	*mJoint;
	LLVector3		mRootToJointSkinOffset;
	LLVector3		mRootToParentJointSkinOffset;
};

//-----------------------------------------------------------------------------
// class LLViewerJointMesh
//-----------------------------------------------------------------------------
class LLViewerJointMesh : public LLViewerJoint
{
protected:
	LLColor4					mColor;			// color value
// 	LLColor4					mSpecular;		// specular color (always white for now)
	F32							mShiny;			// shiny value
	LLPointer<LLViewerImage>	mTexture;		// ptr to a global texture
	LLTexLayerSet*				mLayerSet;		// ptr to a layer set owned by the avatar
	U32 						mTestImageName;		// handle to a temporary texture for previewing uploads
	LLPolyMesh*					mMesh;			// ptr to a global polymesh
	BOOL						mCullBackFaces;	// true by default
	LLFace*						mFace;			// ptr to a face w/ AGP copy of mesh

	U32							mFaceIndexCount;
	BOOL						mIsTransparent;

	U32							mNumSkinJoints;
	LLSkinJoint*				mSkinJoints;
	S32							mMeshID;

public:
	static BOOL					sPipelineRender;
	//RN: this is here for testing purposes
	static U32					sClothingMaskImageName;
	static EAvatarRenderPass	sRenderPass;
	static LLColor4				sClothingInnerColor;

public:
	// Constructor
	LLViewerJointMesh();

	// Destructor
	virtual ~LLViewerJointMesh();

	// Gets the shape color
	void getColor( F32 *red, F32 *green, F32 *blue, F32 *alpha );

	// Sets the shape color
	void setColor( F32 red, F32 green, F32 blue, F32 alpha );

	// Sets the shininess
	void setSpecular( const LLColor4& color, F32 shiny ) { /*mSpecular = color;*/ mShiny = shiny; };

	// Sets the shape texture
	void setTexture( LLViewerImage *texture );

	void setTestTexture( U32 name ) { mTestImageName = name; }

	// Sets layer set responsible for a dynamic shape texture (takes precedence over normal texture)
	void setLayerSet( LLTexLayerSet* layer_set );

	// Gets the poly mesh
	LLPolyMesh *getMesh();

	// Sets the poly mesh
	void setMesh( LLPolyMesh *mesh );

	// Sets up joint matrix data for rendering
	void setupJoint(LLViewerJoint* current_joint);

	// Render time method to upload batches of joint matrices
	void uploadJointMatrices();

	// Sets ID for picking
	void setMeshID( S32 id ) {mMeshID = id;}

	// Gets ID for picking
	S32 getMeshID() { return mMeshID; }	

	// overloaded from base class
	/*virtual*/ void drawBone();
	/*virtual*/ BOOL isTransparent();
	/*virtual*/ U32 drawShape( F32 pixelArea, BOOL first_pass );

	/*virtual*/ void updateFaceSizes(U32 &num_vertices, U32& num_indices, F32 pixel_area);
	/*virtual*/ void updateFaceData(LLFace *face, F32 pixel_area, BOOL damp_wind = FALSE);
	/*virtual*/ BOOL updateLOD(F32 pixel_area, BOOL activate);
	/*virtual*/ void updateGeometry();
	/*virtual*/ void dump();

	void setIsTransparent(BOOL is_transparent) { mIsTransparent = is_transparent; }

	/*virtual*/ BOOL isAnimatable() { return FALSE; }
	void writeCAL3D(apr_file_t* fp, S32 material_num, LLCharacter* characterp);
private:
	// Allocate skin data
	BOOL allocateSkinData( U32 numSkinJoints );

	S32 getBoundJointsByIndex(S32 index, S32 &joint_a, S32& joint_b);

	// Free skin data
	void freeSkinData();
};

#endif // LL_LLVIEWERJOINTMESH_H
