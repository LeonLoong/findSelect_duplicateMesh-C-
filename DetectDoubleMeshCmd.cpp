//
// Copyright (C) 
// 
// File: DetectDoubleMeshCmd.cpp
//
// MEL Command: DetectDoubleMesh
//
// Author: Maya Plug-in Wizard 2.0
//

// Includes everything needed to register a simple MEL command with Maya.
// 
#include <DoubleMesh.h>

#include<maya/MString.h>
#include<maya/MItDag.h>
#include<maya/MBoundingBox.h>
#include<maya/MMatrix.h>
#include<maya/MPointArray.h>
#include<vector>
#include<algorithm>


// Use helper macro to register a command with Maya.  It creates and
// registers a command that does not support undo or redo.  The 
// created class derives off of MPxCommand.
//
MStatus checkDuplicateMesh::doIt(const MArgList& args)
{
	//get the active selection list
	MSelectionList mSelectionList;
	MGlobal::getActiveSelectionList(mSelectionList);
	MGlobal::clearSelectionList();
	MItDag dagIterator(MItDag::kDepthFirst, MFn::kMesh);

	//if nothing is selected
	if (mSelectionList.length() == 0)
	{
		//add all mesh type from scene to selectionList
		while (!dagIterator.isDone())
		{
			//avoiding intermediate objects
			MFnDagNode mfnDagNode(dagIterator.currentItem());
			if (!mfnDagNode.isIntermediateObject())
			{
				mSelectionList.add(dagIterator.fullPathName());
			}
			dagIterator.next();
		}

		//if mesh found in scene
		if (mSelectionList.length() != 0)
		{
			iterateFn(mSelectionList);
		}
		else
		{
			MGlobal::displayInfo("No Mesh present in the scene !!");
		}
	}
	else //if user has selected a group of mesh
	{
		MDagPath selDagPath;
		MSelectionList childSelList;

		for (unsigned int i = 0; i < mSelectionList.length(); i++)
		{
			mSelectionList.getDagPath(i, selDagPath);

			//select all children shapes
			dagIterator.reset(selDagPath, MItDag::kDepthFirst, MFn::kMesh);
			while (!dagIterator.isDone())
			{
				//avoiding intermediate objects
				MFnDagNode mfnDagNode(dagIterator.currentItem());
				if (!mfnDagNode.isIntermediateObject())
				{
					childSelList.add(dagIterator.fullPathName());
				}

				dagIterator.next();
			}
		}

		iterateFn(childSelList);

	}


	return MS::kSuccess;
}

///////////////////////////////////////////////////////////////////////////////////
/// This function iterates through the selectionList passed to it.		///
/// Compares every dagPath with everyother by feeding them to checkDoubleMesh() ///
///////////////////////////////////////////////////////////////////////////////////

void checkDuplicateMesh::iterateFn(MSelectionList& selList)
{
	int value = 0;

	//iterate through selection list and pass pairs to checkDoubleMesh() function

	for (unsigned int i = 0; i < selList.length(); i++)
	{
		for (unsigned int j = i + 1; j < selList.length(); j++)
		{
			MDagPath dagPath1, dagPath2;

			selList.getDagPath(i, dagPath1);
			selList.getDagPath(j, dagPath2);

			//if both dagpaths points to mesh type, check for doubleMesh
			if (dagPath1.apiType() == MFn::kMesh && dagPath2.apiType() == MFn::kMesh)
			{
				value = checkDoubleMesh(dagPath1, dagPath2);
			}

			if (value == 1)
			{
				//select both the transform nodes
				MStringArray dagPathStringArray1;
				MStringArray dagPathStringArray2;
				dagPath1.fullPathName().split('|', dagPathStringArray1);
				dagPath2.fullPathName().split('|', dagPathStringArray2);
				MGlobal::selectByName(dagPathStringArray1[dagPathStringArray1.length() - 2]);
				MGlobal::selectByName(dagPathStringArray2[dagPathStringArray2.length() - 2]);
			}


		}

	}

	MSelectionList finalSelList;
	MGlobal::getActiveSelectionList(finalSelList);

	if (finalSelList.length() == 0)
	{
		MGlobal::displayInfo("No Duplicates found");
	}

}

/////////////////////////////////////////////////////////////////////////////////
/// This function creates two bounding boxes for the dagPaths passed.	      ///
/// Checks if the bounding boxes are at exact same position with a tolerance  ///
/////////////////////////////////////////////////////////////////////////////////

int checkDuplicateMesh::checkDoubleMesh(MDagPath& dagPath1, MDagPath& dagPath2)
{
	double tol = 0.01; //tolerance value

					   ///////////////////////////////////
					   // Make the first worldspace bbox
					   ///////////////////////////////////

	MFnDagNode dagFn1(dagPath1);

	MBoundingBox bbox1 = dagFn1.boundingBox();

	//get worldspace points
	MPoint min1 = bbox1.min() * dagPath1.exclusiveMatrix();
	MPoint max1 = bbox1.max() * dagPath1.exclusiveMatrix();

	//make a worldspace bounding box
	MBoundingBox bBoxWorld1(min1, max1);

	/////////////////////////////////////
	// Make the second worldspace bbox
	/////////////////////////////////////

	MFnDagNode dagFn2(dagPath2);

	MBoundingBox bbox2 = dagFn2.boundingBox();

	//get worldspace points
	MPoint min2 = bbox2.min() * dagPath2.exclusiveMatrix();
	MPoint max2 = bbox2.max() * dagPath2.exclusiveMatrix();

	//make a worldspace bounding box
	MBoundingBox bBoxWorld2(min2, max2);


	//compare the bounding boxes to identify double mesh cases

	if (min1.isEquivalent(min2, tol) && max1.isEquivalent(max2, tol))
	{
		MGlobal::displayInfo("Duplicate Mesh detected");
		MGlobal::displayInfo(dagPath1.fullPathName());
		MGlobal::displayInfo(dagPath2.fullPathName());
		return 1;
	}
	else
		return 0;

}