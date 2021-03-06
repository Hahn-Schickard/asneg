/*
    EventTypeClass: AuditUpdateEventType

    Generated Source Code - please do not change this source code

    EventTypeCodeGenerator Version:
        OpcUaStackCore - 3.0.1

    Autor: Kai Huebl (kai@huebl-sgh.de)
*/

#ifndef __OpcUaStackCore_AuditUpdateEventType_h__
#define __OpcUaStackCore_AuditUpdateEventType_h__

#include <boost/shared_ptr.hpp>
#include "OpcUaStackCore/Base/os.h"
#include "OpcUaStackCore/BuildInTypes/BuildInTypes.h"
#include "OpcUaStackCore/StandardEventType/AuditEventType.h"

namespace OpcUaStackCore
{
    
    class DLLEXPORT AuditUpdateEventType
    : public AuditEventType
    {
      public:
        typedef boost::shared_ptr<AuditUpdateEventType> SPtr;
    
        AuditUpdateEventType(void);
        virtual ~AuditUpdateEventType(void);
        bool setAuditUpdateEventType(OpcUaVariant::SPtr& variable);
        OpcUaVariant::SPtr getAuditUpdateEventType(void);
        
        
        //- EventBase interface
        virtual void mapNamespaceUri(void);
        
        virtual OpcUaVariant::SPtr get(
            OpcUaNodeId& eventType,
            std::list<OpcUaQualifiedName::SPtr>& browseNameList,
            EventResult::Code& resultCode
        );
        //- EventBase interface
        
    
      private:
        EventVariables eventVariables_;
    
    };

}

#endif
