/*
   Copyright 2015 Kai Huebl (kai@huebl-sgh.de)

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

#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaStackClient/ServiceSet/MonitoredItemService.h"

using namespace OpcUaStackCore;

namespace OpcUaStackClient
{

	MonitoredItemService::MonitoredItemService(void)
	: componentSession_(nullptr)
	, monitoredItemServiceIf_(nullptr)
	{
	}

	MonitoredItemService::~MonitoredItemService(void)
	{
	}

	void
	MonitoredItemService::componentSession(Component* componentSession)
	{
		componentSession_ = componentSession;
	}

	void
	MonitoredItemService::monitoredItemServiceIf(MonitoredItemServiceIf* monitoredItemServiceIf)
	{
		monitoredItemServiceIf_ = monitoredItemServiceIf;
	}

	void
	MonitoredItemService::sendSync(ServiceTransactionCreateMonitoredItems::SPtr serviceTransactionCreateMonitoredItems)
	{
		serviceTransactionCreateMonitoredItems->sync(true);
		serviceTransactionCreateMonitoredItems->conditionBool().conditionInit();
		send(serviceTransactionCreateMonitoredItems);
		serviceTransactionCreateMonitoredItems->conditionBool().waitForCondition();
	}

	void
	MonitoredItemService::send(ServiceTransactionCreateMonitoredItems::SPtr serviceTransactionCreateMonitoredItems)
	{
		serviceTransactionCreateMonitoredItems->componentService(this);
		componentSession_->sendAsync(serviceTransactionCreateMonitoredItems);
	}

	void
	MonitoredItemService::sendSync(ServiceTransactionDeleteMonitoredItems::SPtr serviceTransactionDeleteMonitoredItems)
	{
		serviceTransactionDeleteMonitoredItems->sync(true);
		serviceTransactionDeleteMonitoredItems->conditionBool().conditionInit();
		send(serviceTransactionDeleteMonitoredItems);
		serviceTransactionDeleteMonitoredItems->conditionBool().waitForCondition();
	}

	void
	MonitoredItemService::send(ServiceTransactionDeleteMonitoredItems::SPtr serviceTransactionDeleteMonitoredItems)
	{
		serviceTransactionDeleteMonitoredItems->componentService(this);
		componentSession_->sendAsync(serviceTransactionDeleteMonitoredItems);
	}

	void
	MonitoredItemService::sendSync(ServiceTransactionModifyMonitoredItems::SPtr serviceTransactionModifyMonitoredItems)
	{
		serviceTransactionModifyMonitoredItems->sync(true);
		serviceTransactionModifyMonitoredItems->conditionBool().conditionInit();
		send(serviceTransactionModifyMonitoredItems);
		serviceTransactionModifyMonitoredItems->conditionBool().waitForCondition();
	}

	void
	MonitoredItemService::send(ServiceTransactionModifyMonitoredItems::SPtr serviceTransactionModifyMonitoredItems)
	{
		serviceTransactionModifyMonitoredItems->componentService(this);
		componentSession_->sendAsync(serviceTransactionModifyMonitoredItems);
	}

	void
	MonitoredItemService::sendSync(ServiceTransactionSetMonitoringMode::SPtr serviceTransactionSetMonitoringMode)
	{
		serviceTransactionSetMonitoringMode->sync(true);
		serviceTransactionSetMonitoringMode->conditionBool().conditionInit();
		send(serviceTransactionSetMonitoringMode);
		serviceTransactionSetMonitoringMode->conditionBool().waitForCondition();
	}

	void
	MonitoredItemService::send(ServiceTransactionSetMonitoringMode::SPtr serviceTransactionSetMonitoringMode)
	{
		serviceTransactionSetMonitoringMode->componentService(this);
		componentSession_->sendAsync(serviceTransactionSetMonitoringMode);
	}

	void
	MonitoredItemService::sendSync(ServiceTransactionSetTriggering::SPtr serviceTransactionSetTriggering)
	{
		serviceTransactionSetTriggering->sync(true);
		serviceTransactionSetTriggering->conditionBool().conditionInit();
		send(serviceTransactionSetTriggering);
		serviceTransactionSetTriggering->conditionBool().waitForCondition();
	}

	void
	MonitoredItemService::send(ServiceTransactionSetTriggering::SPtr serviceTransactionSetTriggering)
	{
		serviceTransactionSetTriggering->componentService(this);
		componentSession_->sendAsync(serviceTransactionSetTriggering);
	}

	void
	MonitoredItemService::receive(Message::SPtr message)
	{
		ServiceTransaction::SPtr serviceTransaction = boost::static_pointer_cast<ServiceTransaction>(message);

		// check if transaction is synchron
		if (serviceTransaction->sync()) {
			serviceTransaction->conditionBool().conditionTrue();
			return;
		}

		switch (serviceTransaction->nodeTypeResponse().nodeId<uint32_t>())
		{
			case OpcUaId_CreateMonitoredItemsResponse_Encoding_DefaultBinary:
			{
				if (monitoredItemServiceIf_ != nullptr) {
					monitoredItemServiceIf_->monitoredItemServiceCreateMonitoredItemsResponse(
						boost::static_pointer_cast<ServiceTransactionCreateMonitoredItems>(serviceTransaction)
					);
				}
				break;
			}

			case OpcUaId_DeleteMonitoredItemsResponse_Encoding_DefaultBinary:
			{
				if (monitoredItemServiceIf_ != nullptr) {
					monitoredItemServiceIf_->monitoredItemServiceDeleteMonitoredItemsResponse(
						boost::static_pointer_cast<ServiceTransactionDeleteMonitoredItems>(serviceTransaction)
					);
				}
				break;
			}

			case OpcUaId_ModifyMonitoredItemsResponse_Encoding_DefaultBinary:
			{
				if (monitoredItemServiceIf_ != nullptr) {
					monitoredItemServiceIf_->monitoredItemServiceModifyMonitoredItemsResponse(
						boost::static_pointer_cast<ServiceTransactionModifyMonitoredItems>(serviceTransaction)
					);
				}
				break;
			}

			case OpcUaId_SetMonitoringModeResponse_Encoding_DefaultBinary:
			{
				if (monitoredItemServiceIf_ != nullptr) {
					monitoredItemServiceIf_->monitoredItemServiceSetMonitoringModeResponse(
						boost::static_pointer_cast<ServiceTransactionSetMonitoringMode>(serviceTransaction)
					);
				}
				break;
			}

			case OpcUaId_SetTriggeringResponse_Encoding_DefaultBinary:
			{
				if (monitoredItemServiceIf_ != nullptr) {
					monitoredItemServiceIf_->monitoredItemServiceSetTriggeringResponse(
						boost::static_pointer_cast<ServiceTransactionSetTriggering>(serviceTransaction)
					);
				}
				break;
			}

			default:
				Log(Error, "monitored item service received unknown message type")
					.parameter("TypeId", serviceTransaction->nodeTypeRequest());
				break;
		}
	}

}
