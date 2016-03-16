/*
   Copyright 2016 Kai Huebl (kai@huebl-sgh.de)

   Lizenziert gemäß Apache Licence Version 2.0 (die „Lizenz“); Nutzung dieser
   Datei nur in Übereinstimmung mit der Lizenz erlaubt.
   Eine Kopie der Lizenz erhalten Sie auf http://www.apache.org/licenses/LICENSE-2.0.

   Sofern nicht gemäß geltendem Recht vorgeschrieben oder schriftlich vereinbart,
   erfolgt die Bereitstellung der im Rahmen der Lizenz verbreiteten Software OHNE
   GEWÄHR ODER VORBEHALTE – ganz gleich, ob ausdrücklich oder stillschweigend.

   Informationen über die jeweiligen Bedingungen für Genehmigungen und Einschränkungen
   im Rahmen der Lizenz finden Sie in der Lizenz.

   Autor: Kai Huebl (kai@huebl-sgh.de)
 */

#ifndef __OpcUaClient_ClientServiceNodeSet_h__
#define __OpcUaClient_ClientServiceNodeSet_h__

#include <boost/shared_ptr.hpp>
#include "OpcUaStackClient/ServiceSet/ViewServiceBrowse.h"
#include "OpcUaStackClient/ServiceSet/AttributeServiceNode.h"
#include "OpcUaStackServer/AddressSpaceModel/BaseNodeClass.h"
#include "OpcUaStackServer/InformationModel/InformationModel.h"
#include "OpcUaClient/ClientService/ClientServiceBase.h"
#include "OpcUaClient/ClientService/ClientServiceManager.h"

using namespace OpcUaStackServer;

namespace OpcUaClient
{

	class ClientServiceNodeSet
	: public ClientServiceBase
	, public ViewServiceBrowseIf
	, public AttributeServiceNodeIf
	{
	  public:
		typedef boost::shared_ptr<ClientServiceNodeSet> SPtr;

		ClientServiceNodeSet(void);
		virtual ~ClientServiceNodeSet(void);

		//- ClientServiceNodeSet interface ------------------------------------
		virtual ClientServiceBase::SPtr createClientService(void);
		virtual bool run(ClientServiceManager& clientServiceManager, CommandBase::SPtr& commandBase);
		//- ClientServiceNodeSet interface ------------------------------------

		//- ViewServiceBrowseIf -----------------------------------------------
		virtual void viewServiceBrowseDone(OpcUaStatusCode statusCode);
		virtual void viewServiceBrowseResult(
			OpcUaStatusCode statusCode,
			OpcUaNodeId::SPtr& nodeId,
			ReferenceDescription::Vec& referenceDescriptionVec
		);
		//- ViewServiceBrowseIf -----------------------------------------------

		//- AttributeServiceNodeIf --------------------------------------------
		virtual void attributeServiceNodeDone(OpcUaStatusCode statusCode);
		virtual void attributeServiceNodeResult(
			AttributeId attributeId,
			OpcUaDataValue::SPtr& dataValue
		);
		//- AttributeServiceNodeId --------------------------------------------

      private:
		OpcUaStatusCode readNodeAttributes(
			OpcUaNodeId::SPtr& nodeId,
			NodeClassType nodeClassType
		);
		bool createRootNode(OpcUaNodeId& rootNodeId);

		OpcUaNodeId readNodeId_;
		ConditionBool browseCompleted_;
		ConditionBool readCompleted_;
		AttributeService::SPtr attributeService_;
		BaseNodeClass::SPtr baseNodeClass_;
		InformationModel informationModel_;

		OpcUaStatusCode browseStatusCode_;
		OpcUaStatusCode readStatusCode_;
	};

}

#endif

