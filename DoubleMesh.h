#ifndef _DoubleMesh
#define _DoubleMesh

#include <maya/MFnPlugin.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MPxCommand.h>
#include <maya/MSelectionList.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/// This plugin creates two new Maya commands - checkDuplicateFace and checkDuplicateMesh.            ///
/// checkDuplicateMesh - checks for duplicate meshes inside existing mesh.			      ///
/// If found display DagFullPath and select the transform node associated with the duplicate meshes.  ///
/// checkDuplicateFace - checks for duplicate face under a face on the mesh.			      ///
/// If found display the DagFullPath of the faces and select the duplicate faces.		      ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

class checkDuplicateMesh : public MPxCommand
{
public:
	checkDuplicateMesh();
	virtual		~checkDuplicateMesh();

	virtual MStatus doIt(const MArgList& args);
	int checkDoubleMesh(MDagPath& dagPath1, MDagPath& dagPath2);
	void iterateFn(MSelectionList& selList);

	static void* creator();

};

checkDuplicateMesh::checkDuplicateMesh() {}

checkDuplicateMesh::~checkDuplicateMesh() {}

void * checkDuplicateMesh::creator()
{
	return new checkDuplicateMesh;
}




/////////////////////////////////////////////////
// Initialize and Uninitialize the Maya Plugin //
/////////////////////////////////////////////////

MStatus initializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj, "Loong Yin Min, Vaishak Babu", "1.0", "Any");

	status = plugin.registerCommand("checkDuplicateMesh",
		checkDuplicateMesh::creator);
	if (!status) {
		status.perror("registerCommand");

		return status;
	}

	return status;
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj);

	status = plugin.deregisterCommand("checkDuplicateMesh");
	if (!status) {
		status.perror("deregisterCommand");

		return status;
	}

	return status;
}


#endif


