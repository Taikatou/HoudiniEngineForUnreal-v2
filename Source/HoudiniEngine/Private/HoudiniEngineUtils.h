/*
* Copyright (c) <2018> Side Effects Software Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. The name of Side Effects Software may not be used to endorse or
*    promote products derived from this software without specific prior
*    written permission.
*
* THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
* NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "HAPI/HAPI_Common.h"
#include "HoudiniEnginePrivatePCH.h"
#include <string>

class FString;
class UStaticMesh;
class UHoudiniAsset;
class UHoudiniAssetComponent;

struct FHoudiniPartInfo;
struct FHoudiniMeshSocket;
struct FHoudiniGeoPartObject;
struct FHoudiniGenericAttribute;

struct FRawMesh;

enum class EHoudiniCurveType : int8;
enum class EHoudiniCurveMethod : int8;
enum class EHoudiniInstancerType : uint8;

struct HOUDINIENGINE_API FHoudiniEngineUtils
{
	friend struct FUnrealMeshTranslator;

	public:
		// Load libHAPI and return handle to it, also store location of loaded libHAPI in passed argument.
		static void* LoadLibHAPI(FString& StoredLibHAPILocation);

		// Return true if module has been properly initialized.
		static bool IsInitialized();

		// Return type of license used.
		static bool GetLicenseType(FString & LicenseType);

		// Return a specified HAPI status string.
		static const FString GetStatusString(HAPI_StatusType status_type, HAPI_StatusVerbosity verbosity);

		// Return a string representing cooking result.
		static const FString GetCookResult();

		// Return a string indicating cook state.
		static const FString GetCookState();

		// Return a string error description.
		static const FString GetErrorDescription();

		// Return a string description of error from a given error code.
		static const FString GetErrorDescription(HAPI_Result Result);

		// Return the errors, warning and messages on a specified node
		static const FString GetNodeErrorsWarningsAndMessages(const HAPI_NodeId& InNodeId);

		static const FString GetCookLog(TArray<UHoudiniAssetComponent*>& InHACs);

		static const FString GetAssetHelp(UHoudiniAssetComponent* HoudiniAssetComponent);

		// Updates the Object transform of a Houdini Asset Component
		static bool UploadHACTransform(UHoudiniAssetComponent* HAC);

		// Convert FString to std::string
		static void ConvertUnrealString(const FString & UnrealString, std::string& String);
		
		// Wrapper for the CreateNode function
		// As HAPI_CreateNode is an async call, this function actually waits for the node creation to be done before returning
		static HAPI_Result CreateNode(
			const HAPI_NodeId& InParentNodeId, 
			const char * operator_name, 
			const char * node_label, 
			const HAPI_Bool& bInCookOnCreation, 
			HAPI_NodeId* OutNewNodeId);

		static int32 HapiGetCookCount(const HAPI_NodeId& InNodeId);

		// HAPI : Retrieve the asset node's object transform. **/
		static bool HapiGetAssetTransform(const HAPI_NodeId& InNodeId, FTransform& OutTransform);

		// HAPI : Retrieve object transforms from given asset node id.
		static bool HapiGetObjectTransforms(const HAPI_NodeId& InNodeId, TArray<HAPI_Transform>& OutObjectTransforms);

		// HAPI : Translate HAPI transform to Unreal one.
		static void TranslateHapiTransform(const HAPI_Transform & HapiTransform, FTransform & UnrealTransform);

		// HAPI : Translate HAPI Euler transform to Unreal one.
		static void TranslateHapiTransform(const HAPI_TransformEuler & HapiTransformEuler, FTransform & UnrealTransform);

		// HAPI : Translate Unreal transform to HAPI one.
		static void TranslateUnrealTransform(const FTransform & UnrealTransform, HAPI_Transform & HapiTransform);

		// HAPI : Translate Unreal transform to HAPI Euler one.
		static void TranslateUnrealTransform(const FTransform & UnrealTransform, HAPI_TransformEuler & HapiTransformEuler);

		// Return true if asset is valid.
		static bool IsHoudiniNodeValid(const HAPI_NodeId& AssetId);

		// HAPI : Retrieve HAPI_ObjectInfo's from given asset node id.
		static bool HapiGetObjectInfos(const HAPI_NodeId& InNodeId, TArray<HAPI_ObjectInfo>& OutObjectInfos);

		// HAPI: Retrieve Path to the given Node, relative to the given Node
		static bool HapiGetNodePath(const HAPI_NodeId& InNodeId, const HAPI_NodeId& InRelativeToNodeId, FString& OutPath);

		// HAPI: Retrieve the relative for the given HGPO Node
		static bool HapiGetNodePath(const FHoudiniGeoPartObject& InHGPO, FString& OutPath);

		// HAPI : Return all group names for a given Geo. 
		static bool HapiGetGroupNames(
			const HAPI_NodeId& GeoId, const HAPI_PartId& PartId,
			const HAPI_GroupType& GroupType, const bool& isPackedPrim,
			TArray<FString>& OutGroupNames );

		// HAPI : Retrieve group membership.
		static bool HapiGetGroupMembership(
			const HAPI_NodeId& GeoId, const HAPI_PartInfo& PartInfo,
			const HAPI_GroupType& GroupType, const FString & GroupName,
			TArray<int32>& OutGroupMembership, bool& OutAllEquals);

		// HAPI : Given vertex list, retrieve new vertex list for a specified group.
		// Return number of processed valid index vertices for this split.
		static int32 HapiGetVertexListForGroup(
			const HAPI_NodeId& GeoId,
			const HAPI_PartInfo& PartInfo,
			const FString& GroupName,
			const TArray<int32>& FullVertexList,
			TArray<int32>& NewVertexList,
			TArray<int32>& AllVertexList,
			TArray<int32>& AllFaceList,
			TArray<int32>& AllGroupFaceIndices,
			int32& FirstValidVertex,
			int32& FirstValidPrim,
			const bool& isPackedPrim);

		// HAPI : Get attribute data as float.
		static bool HapiGetAttributeDataAsFloat(
			const HAPI_NodeId& InGeoId,
			const HAPI_PartId& InPartId,
			const char * InAttribName,
			HAPI_AttributeInfo& OutAttributeInfo,
			TArray<float>& OutData,
			int32 InTupleSize = 0,
			HAPI_AttributeOwner InOwner = HAPI_ATTROWNER_INVALID);

		// HAPI : Get attribute data as Integer.
		static bool HapiGetAttributeDataAsInteger(
			const HAPI_NodeId& InGeoId,
			const HAPI_PartId& InPartId,
			const char * InAttribName,
			HAPI_AttributeInfo& OutAttributeInfo,
			TArray<int32>& OutData,
			const int32& InTupleSize = 0,
			const HAPI_AttributeOwner& InOwner = HAPI_ATTROWNER_INVALID);

		// HAPI : Get attribute data as strings.
		static bool HapiGetAttributeDataAsString(
			const HAPI_NodeId& InGeoId,
			const HAPI_PartId& InPartId,
			const char * InAttribName,
			HAPI_AttributeInfo& OutAttributeInfo,
			TArray<FString>& OutData,
			int32 InTupleSize = 0,
			HAPI_AttributeOwner InOwner = HAPI_ATTROWNER_INVALID);

		// HAPI : Get attribute data as strings.
		static bool HapiGetAttributeDataAsStringFromInfo(
			const HAPI_NodeId& InGeoId,
			const HAPI_PartId& InPartId,
			HAPI_AttributeInfo& InAttributeInfo,
			const char * InAttribName,
			TArray<FString>& OutData);

		// HAPI : Check if given attribute exists.
		static bool HapiCheckAttributeExists(
			const HAPI_NodeId& GeoId,
			const HAPI_PartId& PartId,
			const char * AttribName,
			HAPI_AttributeOwner Owner = HAPI_ATTROWNER_INVALID);

		// HAPI: Returns all the attributes of a given type for a given owner
		static int32 HapiGetAttributeOfType(
			const HAPI_NodeId& GeoId,
			const HAPI_NodeId& PartId,
			const HAPI_AttributeOwner& AttributeOwner,
			const HAPI_AttributeTypeInfo& AttributeType,
			TArray<HAPI_AttributeInfo>& MatchingAttributesInfo,
			TArray<FString>& MatchingAttributesName);

		// HAPI : Look for a parameter by name or tag and returns its index. Returns -1 if not found.
		static HAPI_ParmId HapiFindParameterByNameOrTag(
			const HAPI_NodeId& NodeId, const std::string& ParmName);
		static HAPI_ParmId HapiFindParameterByNameOrTag(
			const HAPI_NodeId& NodeId, const std::string& ParmName, HAPI_ParmInfo& FoundParmInfo);

		// Returns true is the given Geo-Part is an attribute instancer
		static bool IsAttributeInstancer(const HAPI_NodeId& GeoId, const HAPI_PartId& PartId, EHoudiniInstancerType& OutInstancerType);

		// Return true if given asset id is valid. 
		//static bool IsValidNodeId(HAPI_NodeId AssetId);

		// HAPI : Return a give node's parent ID, -1 if none
		static HAPI_NodeId HapiGetParentNodeId(const HAPI_NodeId& NodeId);

		/** HAPI : Marshaling, disconnect input asset from a given slot. **/
		static bool HapiDisconnectAsset(HAPI_NodeId HostAssetId, int32 InputIndex);

		// Destroy asset, returns the status.
		static bool DestroyHoudiniAsset(const HAPI_NodeId& AssetId);

		// Loads an HDA file and returns its AssetLibraryId
		static bool LoadHoudiniAsset(
			UHoudiniAsset * HoudiniAsset,
			HAPI_AssetLibraryId & OutAssetLibraryId);
		
		// Returns the name of the available subassets in a loaded HDA
		static bool GetSubAssetNames(
			const HAPI_AssetLibraryId& AssetLibraryId,
			TArray< HAPI_StringHandle > & OutAssetNames);

		static bool OpenSubassetSelectionWindow(
			TArray<HAPI_StringHandle>& AssetNames, HAPI_StringHandle& OutPickedAssetName );

		// Returns the name of a Houdini asset.
		static bool GetHoudiniAssetName(const HAPI_NodeId& AssetNodeId, FString & NameString);

		// Gets preset data for a given asset.
		static bool GetAssetPreset(const HAPI_NodeId& AssetNodeId, TArray< char > & PresetBuffer);

		// HAPI : Set asset transform.
		static bool HapiSetAssetTransform(const HAPI_NodeId& AssetNodeId, const FTransform & Transform);

		// TODO: Move me somewhere else
		static void AssignUniqueActorLabelIfNeeded(UHoudiniAssetComponent* HAC);

		// Triggers an update the details panel
		// Will use an AsyncTask if we're not in the game thread
		static void UpdateEditorProperties(UObject* InObjectToUpdate, const bool& InForceFullUpdate);

		// Triggers an update the details panel
		// Will use an AsyncTask if we're not in the game thread
		static void UpdateEditorProperties(TArray<UObject*> InObjectsToUpdate, const bool& InForceFullUpdate);

		// Check if the Houdini asset component is being cooked
		static bool IsHoudiniAssetComponentCooking(UObject* InObj);

		// Helper function to set attribute string data for a single FString
		static HAPI_Result SetAttributeStringData(
			const FString& InString,
			const HAPI_NodeId& InNodeId,
			const HAPI_PartId& InPartId,
			const FString& InAttributeName,
			const HAPI_AttributeInfo& InAttributeInfo);

		// Helper function to set attribute string data for a FString array
		static HAPI_Result SetAttributeStringData(
			const TArray<FString>& InStringArray,
			const HAPI_NodeId& InNodeId,
			const HAPI_PartId& InPartId,
			const FString& InAttributeName,
			const HAPI_AttributeInfo& InAttributeInfo);

		static bool HapiGetParameterDataAsString(
			const HAPI_NodeId& NodeId,
			const std::string& ParmName,
			const FString& DefaultValue,
			FString& OutValue);

		static bool HapiGetParameterDataAsInteger(
			const HAPI_NodeId& NodeId, 
			const std::string& ParmName,
			const int32& DefaultValue,
			int32 & OutValue);

		static bool HapiGetParameterDataAsFloat(
			const HAPI_NodeId& NodeId,
			const std::string& ParmName,
			const float& DefaultValue,
			float& OutValue);
		
		// Returns a list with all the Property attributes found on a HGPO
		static int32 GetPropertyAttributeList(
			const FHoudiniGeoPartObject& InHGPO, TArray<FHoudiniGenericAttribute>& OutFoundUProps);

		// Updates all FProperty attributes found on a given object
		static void UpdateAllPropertyAttributesOnObject(
			UObject* InObject, const FHoudiniGeoPartObject& InHGPO);

		// Returns a list of all the generic attributes for a given attribute owner
		static int32 GetGenericAttributeList(
			const HAPI_NodeId& InGeoNodeId,
			const HAPI_PartId& InPartId,
			const FString& InGenericAttributePrefix,
			TArray<FHoudiniGenericAttribute>& OutFoundAttributes,
			const HAPI_AttributeOwner& AttributeOwner,
			const int32& InAttribIndex = -1);

		/*
		// Tries to update values for all the UProperty attributes to apply on the object.
		static void ApplyUPropertyAttributesOnObject(
			UObject* MeshComponent, const TArray< UGenericAttribute >& UPropertiesToModify );
		*/
		/*
		static bool TryToFindInStructProperty(
			UObject* Object, FString UPropertyNameToFind, UStructProperty* ArrayProperty, UProperty*& FoundProperty, void*& StructContainer );
		*/		
		/*
		static bool TryToFindInArrayProperty(
			UObject* Object, FString UPropertyNameToFind, UArrayProperty* ArrayProperty, UProperty*& FoundProperty, void*& StructContainer );
		*/

		static void AddHoudiniMetaInformationToPackage(
			UPackage* Package, UObject* Object, const FString& Key, const FString& Value);

		// Adds the HoudiniLogo mesh to a Houdini Asset Component
		static bool AddHoudiniLogoToComponent(UHoudiniAssetComponent* HAC);

		// Removes the default Houdini logo mesh from a HAC
		static bool RemoveHoudiniLogoFromComponent(UHoudiniAssetComponent* HAC);

		// Indicates if a HAC has the Houdini logo mesh
		static bool HasHoudiniLogo(UHoudiniAssetComponent* HAC);

		// 
		static HAPI_PartInfo ToHAPIPartInfo(const FHoudiniPartInfo& InHPartInfo);

		//
		static int32 AddMeshSocketsToArray_Group(
			const HAPI_NodeId& GeoId, const HAPI_PartId& PartId,
			TArray<FHoudiniMeshSocket>& AllSockets, const bool& isPackedPrim);

		//
		static int32 AddMeshSocketsToArray_DetailAttribute(
			const HAPI_NodeId& GeoId, const HAPI_PartId& PartId,
			TArray<FHoudiniMeshSocket>& AllSockets, const bool& isPackedPrim);

		static bool AddMeshSocketsToStaticMesh(
			UStaticMesh* StaticMesh,
			TArray<FHoudiniMeshSocket >& AllSockets,
			const bool& CleanImportSockets);

		// 
		static bool CreateGroupsFromTags(
			const HAPI_NodeId& NodeId, const HAPI_PartId& PartId, const TArray<FName>& Tags);

		//
		static bool CreateAttributesFromTags(
			const HAPI_NodeId& NodeId, const HAPI_PartId& PartId, const TArray<FName>& Tags);

		static bool GetUnrealTagAttributes(const HAPI_NodeId& GeoId, const HAPI_PartId& PartId, TArray<FName>& OutTags);

		// Helper function used to extract a const char* from a FString
		// !! Allocates memory using malloc that will need to be freed afterwards!
		static char * ExtractRawString(const FString& Name);

		// Frees memory allocated by ExtractRawString()
		static void FreeRawStringMemory(const char*& InRawString);

		// Frees memory allocated by ExtractRawString()
		static void FreeRawStringMemory(TArray<const char*>& InRawStringArray);

		// Make sure a string complies with Houdini's internal variable naming convention (group, attr etc..)
		static bool SanitizeHAPIVariableName(FString& String);

		/** How many GUID symbols are used for package component name generation. **/
		static const int32 PackageGUIDComponentNameLength;

		/** How many GUID symbols are used for package item name generation. **/
		static const int32 PackageGUIDItemNameLength;

		/** Helper routine to check invalid lightmap faces. **/
		static bool ContainsInvalidLightmapFaces(const FRawMesh & RawMesh, int32 LightmapSourceIdx);

		// Helper function for creating a temporary Slate notification.
		static void CreateSlateNotification(
			const FString& NotificationString,
			const float& NotificationExpire = HAPI_UNREAL_NOTIFICATION_EXPIRE,
			const float& NotificationFadeOut = HAPI_UNREAL_NOTIFICATION_FADEOUT);

		static FString GetHoudiniEnginePluginDir();

	protected:
		
		// Computes the XX.YY.ZZZ version string using HAPI_Version
		static FString ComputeVersionString(bool ExtraDigit);

#if PLATFORM_WINDOWS
		// Attempt to locate libHAPI on Windows in the registry. Return handle if located and return location.
		static void* LocateLibHAPIInRegistry(
			const FString& HoudiniInstallationType, FString& StoredLibHAPILocation, bool LookIn32bitRegistry);
#endif

		// Triggers an update the details panel
		//static void UpdateEditorProperties_Internal(UObject* ObjectToUpdate, const bool& bInForceFullUpdate);

		// Triggers an update the details panel
		static void UpdateEditorProperties_Internal(TArray<UObject*> ObjectsToUpdate, const bool& bInForceFullUpdate);

};