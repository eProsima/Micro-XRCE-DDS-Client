#include <micrortps/client/core/serialization/xrce_protocol.h>

//==================================================================
//                             PUBLIC
//==================================================================
bool serialize_Time_t(MicroBuffer* buffer, const Time_t* input)
{
    bool ret = true;
    ret &= serialize_int32_t(buffer, input->seconds);
    ret &= serialize_uint32_t(buffer, input->nanoseconds);
    return ret;
}

bool deserialize_Time_t(MicroBuffer* buffer, Time_t* output)
{
    bool ret = true;
    ret &= deserialize_int32_t(buffer, &output->seconds);
    ret &= deserialize_uint32_t(buffer, &output->nanoseconds);
    return ret;
}

bool serialize_String_t(MicroBuffer* buffer, const String_t* input)
{
    return serialize_sequence_char(buffer, input->data, input->size);
}

bool deserialize_String_t(MicroBuffer* buffer, String_t* output)
{
    return deserialize_sequence_char(buffer, &output->data, &output->size);
}

bool serialize_BinarySequence_t(MicroBuffer* buffer, const BinarySequence_t* input)
{
    return serialize_sequence_uint8_t(buffer, input->data, input->size);
}

bool deserialize_BinarySequence_t(MicroBuffer* buffer, BinarySequence_t* output)
{
    return deserialize_sequence_uint8_t(buffer, &output->data, &output->size);
}

bool serialize_StringSequence_t(MicroBuffer* buffer, const StringSequence_t* input)
{
    bool ret = serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size && ret; i++)
    {
        ret = serialize_String_t(buffer, &input->data[i]);
    }
    return ret;
}

bool deserialize_StringSequence_t(MicroBuffer* buffer, StringSequence_t* output)
{
    bool ret = deserialize_uint32_t(buffer, &output->size);
    for(uint32_t i = 0; i < output->size && ret; i++)
    {
        ret = deserialize_String_t(buffer, &output->data[i]);
    }
    return ret;
}

bool serialize_ClientKey(MicroBuffer* buffer, const ClientKey* input)
{
    bool ret = true;
    ret &= serialize_array_uint8_t(buffer, input->data, 4);
    return ret;
}

bool deserialize_ClientKey(MicroBuffer* buffer, ClientKey* output)
{
    bool ret = true;
    ret &= deserialize_array_uint8_t(buffer, output->data, 4);
    return ret;
}

bool serialize_ObjectId(MicroBuffer* buffer, const ObjectId* input)
{
    bool ret = true;
    ret &= serialize_array_uint8_t(buffer, input->data, 2);
    return ret;
}

bool deserialize_ObjectId(MicroBuffer* buffer, ObjectId* output)
{
    bool ret = true;
    ret &= deserialize_array_uint8_t(buffer, output->data, 2);
    return ret;
}

bool serialize_ObjectPrefix(MicroBuffer* buffer, const ObjectPrefix* input)
{
    bool ret = true;
    ret &= serialize_array_uint8_t(buffer, input->data, 2);
    return ret;
}

bool deserialize_ObjectPrefix(MicroBuffer* buffer, ObjectPrefix* output)
{
    bool ret = true;
    ret &= deserialize_array_uint8_t(buffer, output->data, 2);
    return ret;
}

bool serialize_XrceCookie(MicroBuffer* buffer, const XrceCookie* input)
{
    bool ret = true;
    ret &= serialize_array_uint8_t(buffer, input->data, 4);
    return ret;
}

bool deserialize_XrceCookie(MicroBuffer* buffer, XrceCookie* output)
{
    bool ret = true;
    ret &= deserialize_array_uint8_t(buffer, output->data, 4);
    return ret;
}

bool serialize_XrceVersion(MicroBuffer* buffer, const XrceVersion* input)
{
    bool ret = true;
    ret &= serialize_array_uint8_t(buffer, input->data, 2);
    return ret;
}

bool deserialize_XrceVersion(MicroBuffer* buffer, XrceVersion* output)
{
    bool ret = true;
    ret &= deserialize_array_uint8_t(buffer, output->data, 2);
    return ret;
}

bool serialize_XrceVendorId(MicroBuffer* buffer, const XrceVendorId* input)
{
    bool ret = true;
    ret &= serialize_array_uint8_t(buffer, input->data, 2);
    return ret;
}

bool deserialize_XrceVendorId(MicroBuffer* buffer, XrceVendorId* output)
{
    bool ret = true;
    ret &= deserialize_array_uint8_t(buffer, output->data, 2);
    return ret;
}

bool serialize_TransportLocatorSmall(MicroBuffer* buffer, const TransportLocatorSmall* input)
{
    bool ret = true;
    ret &= serialize_array_uint8_t(buffer, input->address, 2);
    ret &= serialize_uint8_t(buffer, input->locator_port);
    return ret;
}

bool deserialize_TransportLocatorSmall(MicroBuffer* buffer, TransportLocatorSmall* output)
{
    bool ret = true;
    ret &= deserialize_array_uint8_t(buffer, output->address, 2);
    ret &= deserialize_uint8_t(buffer, &output->locator_port);
    return ret;
}

bool serialize_TransportLocatorMedium(MicroBuffer* buffer, const TransportLocatorMedium* input)
{
    bool ret = true;
    ret &= serialize_array_uint8_t(buffer, input->address, 4);
    ret &= serialize_uint8_t(buffer, input->locator_port);
    return ret;
}

bool deserialize_TransportLocatorMedium(MicroBuffer* buffer, TransportLocatorMedium* output)
{
    bool ret = true;
    ret &= deserialize_array_uint8_t(buffer, output->address, 4);
    ret &= deserialize_uint8_t(buffer, &output->locator_port);
    return ret;
}

bool serialize_TransportLocatorLarge(MicroBuffer* buffer, const TransportLocatorLarge* input)
{
    bool ret = true;
    ret &= serialize_array_uint8_t(buffer, input->address, 16);
    ret &= serialize_uint8_t(buffer, input->locator_port);
    return ret;
}

bool deserialize_TransportLocatorLarge(MicroBuffer* buffer, TransportLocatorLarge* output)
{
    bool ret = true;
    ret &= deserialize_array_uint8_t(buffer, output->address, 16);
    ret &= deserialize_uint8_t(buffer, &output->locator_port);
    return ret;
}

bool serialize_TransportLocatorString(MicroBuffer* buffer, const TransportLocatorString* input)
{
    bool ret = true;
    ret &= serialize_String_t(buffer, &input->value);
    return ret;
}

bool deserialize_TransportLocatorString(MicroBuffer* buffer, TransportLocatorString* output)
{
    bool ret = true;
    ret &= deserialize_String_t(buffer, &output->value);
    return ret;
}

bool serialize_TransportLocator(MicroBuffer* buffer, const TransportLocator* input)
{
    bool ret = true;
    ret &= serialize_uint8_t(buffer, input->format);
    if (ret)
    {
        switch(input->format)
        {
            case ADDRESS_FORMAT_SMALL:
                ret &= serialize_TransportLocatorSmall(buffer, &input->_.small_locator);
                break;
            case ADDRESS_FORMAT_MEDIUM:
                ret &= serialize_TransportLocatorMedium(buffer, &input->_.medium_locator);
                break;
            case ADDRESS_FORMAT_LARGE:
                ret &= serialize_TransportLocatorLarge(buffer, &input->_.large_locator);
                break;
            case ADDRESS_FORMAT_STRING:
                ret &= serialize_TransportLocatorString(buffer, &input->_.string_locator);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool deserialize_TransportLocator(MicroBuffer* buffer, TransportLocator* output)
{
    bool ret = true;
    ret &= deserialize_uint8_t(buffer, &output->format);
    if (ret)
    {
        switch(output->format)
        {
            case ADDRESS_FORMAT_SMALL:
                ret &= deserialize_TransportLocatorSmall(buffer, &output->_.small_locator);
                break;
            case ADDRESS_FORMAT_MEDIUM:
                ret &= deserialize_TransportLocatorMedium(buffer, &output->_.medium_locator);
                break;
            case ADDRESS_FORMAT_LARGE:
                ret &= deserialize_TransportLocatorLarge(buffer, &output->_.large_locator);
                break;
            case ADDRESS_FORMAT_STRING:
                ret &= deserialize_TransportLocatorString(buffer, &output->_.string_locator);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool serialize_TransportLocatorSeq(MicroBuffer* buffer, const TransportLocatorSeq* input)
{
    bool ret = serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size && ret; i++)
    {
        ret = serialize_TransportLocator(buffer, &input->data[i]);
    }
    return ret;
}

bool deserialize_TransportLocatorSeq(MicroBuffer* buffer, TransportLocatorSeq* output)
{
    bool ret = deserialize_uint32_t(buffer, &output->size);
    for(uint32_t i = 0; i < output->size && ret; i++)
    {
        ret = deserialize_TransportLocator(buffer, &output->data[i]);
    }
    return ret;
}

bool serialize_Property(MicroBuffer* buffer, const Property* input)
{
    bool ret = true;
    ret &= serialize_String_t(buffer, &input->name);
    ret &= serialize_String_t(buffer, &input->value);
    return ret;
}

bool deserialize_Property(MicroBuffer* buffer, Property* output)
{
    bool ret = true;
    ret &= deserialize_String_t(buffer, &output->name);
    ret &= deserialize_String_t(buffer, &output->value);
    return ret;
}

bool serialize_PropertySeq(MicroBuffer* buffer, const PropertySeq* input)
{
    bool ret = serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size && ret; i++)
    {
        ret = serialize_Property(buffer, &input->data[i]);
    }
    return ret;
}

bool deserialize_PropertySeq(MicroBuffer* buffer, PropertySeq* output)
{
    bool ret = deserialize_uint32_t(buffer, &output->size);
    for(uint32_t i = 0; i < output->size && ret; i++)
    {
        ret = deserialize_Property(buffer, &output->data[i]);
    }
    return ret;
}

bool serialize_CLIENT_Representation(MicroBuffer* buffer, const CLIENT_Representation* input)
{
    bool ret = true;
    ret &= serialize_XrceCookie(buffer, &input->xrce_cookie);
    ret &= serialize_XrceVersion(buffer, &input->xrce_version);
    ret &= serialize_XrceVendorId(buffer, &input->xrce_vendor_id);
    ret &= serialize_Time_t(buffer, &input->client_timestamp);
    ret &= serialize_ClientKey(buffer, &input->client_key);
    ret &= serialize_uint8_t(buffer, input->session_id);
    ret &= serialize_bool(buffer, input->optional_properties);
    if(input->optional_properties == true)
    {
        ret &= serialize_PropertySeq(buffer, &input->properties);
    }

    return ret;
}

bool deserialize_CLIENT_Representation(MicroBuffer* buffer, CLIENT_Representation* output)
{
    bool ret = true;
    ret &= deserialize_XrceCookie(buffer, &output->xrce_cookie);
    ret &= deserialize_XrceVersion(buffer, &output->xrce_version);
    ret &= deserialize_XrceVendorId(buffer, &output->xrce_vendor_id);
    ret &= deserialize_Time_t(buffer, &output->client_timestamp);
    ret &= deserialize_ClientKey(buffer, &output->client_key);
    ret &= deserialize_uint8_t(buffer, &output->session_id);
    ret &= deserialize_bool(buffer, &output->optional_properties);
    if(output->optional_properties == true)
    {
            ret &= deserialize_PropertySeq(buffer, &output->properties);
    }

    return ret;
}

bool serialize_AGENT_Representation(MicroBuffer* buffer, const AGENT_Representation* input)
{
    bool ret = true;
    ret &= serialize_XrceCookie(buffer, &input->xrce_cookie);
    ret &= serialize_XrceVersion(buffer, &input->xrce_version);
    ret &= serialize_XrceVendorId(buffer, &input->xrce_vendor_id);
    ret &= serialize_Time_t(buffer, &input->agent_timestamp);
    ret &= serialize_bool(buffer, input->optional_properties);
    if(input->optional_properties == true)
    {
        ret &= serialize_PropertySeq(buffer, &input->properties);
    }

    return ret;
}

bool deserialize_AGENT_Representation(MicroBuffer* buffer, AGENT_Representation* output)
{
    bool ret = true;
    ret &= deserialize_XrceCookie(buffer, &output->xrce_cookie);
    ret &= deserialize_XrceVersion(buffer, &output->xrce_version);
    ret &= deserialize_XrceVendorId(buffer, &output->xrce_vendor_id);
    ret &= deserialize_Time_t(buffer, &output->agent_timestamp);
    ret &= deserialize_bool(buffer, &output->optional_properties);
    if(output->optional_properties == true)
    {
            ret &= deserialize_PropertySeq(buffer, &output->properties);
    }

    return ret;
}

bool serialize_OBJK_Representation3Formats(MicroBuffer* buffer, const OBJK_Representation3Formats* input)
{
    bool ret = true;
    ret &= serialize_uint8_t(buffer, input->format);
    if (ret)
    {
        switch(input->format)
        {
            case REPRESENTATION_BY_REFERENCE:
                ret &= serialize_String_t(buffer, &input->_.object_reference);
                break;
            case REPRESENTATION_AS_XML_STRING:
                ret &= serialize_String_t(buffer, &input->_.xml_string_represenatation);
                break;
            case REPRESENTATION_IN_BINARY:
                ret &= serialize_BinarySequence_t(buffer, &input->_.binary_representation);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool deserialize_OBJK_Representation3Formats(MicroBuffer* buffer, OBJK_Representation3Formats* output)
{
    bool ret = true;
    ret &= deserialize_uint8_t(buffer, &output->format);
    if (ret)
    {
        switch(output->format)
        {
            case REPRESENTATION_BY_REFERENCE:
                ret &= deserialize_String_t(buffer, &output->_.object_reference);
                break;
            case REPRESENTATION_AS_XML_STRING:
                ret &= deserialize_String_t(buffer, &output->_.xml_string_represenatation);
                break;
            case REPRESENTATION_IN_BINARY:
                ret &= deserialize_BinarySequence_t(buffer, &output->_.binary_representation);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool serialize_OBJK_RepresentationRefAndXMLFormats(MicroBuffer* buffer, const OBJK_RepresentationRefAndXMLFormats* input)
{
    bool ret = true;
    ret &= serialize_uint8_t(buffer, input->format);
    if (ret)
    {
        switch(input->format)
        {
            case REPRESENTATION_BY_REFERENCE:
                ret &= serialize_String_t(buffer, &input->_.object_name);
                break;
            case REPRESENTATION_AS_XML_STRING:
                ret &= serialize_String_t(buffer, &input->_.xml_string_represenatation);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool deserialize_OBJK_RepresentationRefAndXMLFormats(MicroBuffer* buffer, OBJK_RepresentationRefAndXMLFormats* output)
{
    bool ret = true;
    ret &= deserialize_uint8_t(buffer, &output->format);
    if (ret)
    {
        switch(output->format)
        {
            case REPRESENTATION_BY_REFERENCE:
                ret &= deserialize_String_t(buffer, &output->_.object_name);
                break;
            case REPRESENTATION_AS_XML_STRING:
                ret &= deserialize_String_t(buffer, &output->_.xml_string_represenatation);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool serialize_OBJK_RepresentationBinAndXMLFormats(MicroBuffer* buffer, const OBJK_RepresentationBinAndXMLFormats* input)
{
    bool ret = true;
    ret &= serialize_uint8_t(buffer, input->format);
    if (ret)
    {
        switch(input->format)
        {
            case REPRESENTATION_IN_BINARY:
                ret &= serialize_BinarySequence_t(buffer, &input->_.binary_representation);
                break;
            case REPRESENTATION_AS_XML_STRING:
                ret &= serialize_String_t(buffer, &input->_.string_represenatation);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool deserialize_OBJK_RepresentationBinAndXMLFormats(MicroBuffer* buffer, OBJK_RepresentationBinAndXMLFormats* output)
{
    bool ret = true;
    ret &= deserialize_uint8_t(buffer, &output->format);
    if (ret)
    {
        switch(output->format)
        {
            case REPRESENTATION_IN_BINARY:
                ret &= deserialize_BinarySequence_t(buffer, &output->_.binary_representation);
                break;
            case REPRESENTATION_AS_XML_STRING:
                ret &= deserialize_String_t(buffer, &output->_.string_represenatation);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool serialize_OBJK_RepresentationRefAndXML_Base(MicroBuffer* buffer, const OBJK_RepresentationRefAndXML_Base* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationRefAndXMLFormats(buffer, &input->representation);
    return ret;
}

bool deserialize_OBJK_RepresentationRefAndXML_Base(MicroBuffer* buffer, OBJK_RepresentationRefAndXML_Base* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationRefAndXMLFormats(buffer, &output->representation);
    return ret;
}

bool serialize_OBJK_RepresentationBinAndXML_Base(MicroBuffer* buffer, const OBJK_RepresentationBinAndXML_Base* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationBinAndXMLFormats(buffer, &input->representation);
    return ret;
}

bool deserialize_OBJK_RepresentationBinAndXML_Base(MicroBuffer* buffer, OBJK_RepresentationBinAndXML_Base* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationBinAndXMLFormats(buffer, &output->representation);
    return ret;
}

bool serialize_OBJK_Representation3_Base(MicroBuffer* buffer, const OBJK_Representation3_Base* input)
{
    bool ret = true;
    ret &= serialize_OBJK_Representation3Formats(buffer, &input->representation);
    return ret;
}

bool deserialize_OBJK_Representation3_Base(MicroBuffer* buffer, OBJK_Representation3_Base* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_Representation3Formats(buffer, &output->representation);
    return ret;
}

bool serialize_OBJK_QOSPROFILE_Representation(MicroBuffer* buffer, const OBJK_QOSPROFILE_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationRefAndXML_Base(buffer, &input->base);
    return ret;
}

bool deserialize_OBJK_QOSPROFILE_Representation(MicroBuffer* buffer, OBJK_QOSPROFILE_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationRefAndXML_Base(buffer, &output->base);
    return ret;
}

bool serialize_OBJK_TYPE_Representation(MicroBuffer* buffer, const OBJK_TYPE_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationRefAndXML_Base(buffer, &input->base);
    return ret;
}

bool deserialize_OBJK_TYPE_Representation(MicroBuffer* buffer, OBJK_TYPE_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationRefAndXML_Base(buffer, &output->base);
    return ret;
}

bool serialize_OBJK_DOMAIN_Representation(MicroBuffer* buffer, const OBJK_DOMAIN_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationRefAndXML_Base(buffer, &input->base);
    return ret;
}

bool deserialize_OBJK_DOMAIN_Representation(MicroBuffer* buffer, OBJK_DOMAIN_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationRefAndXML_Base(buffer, &output->base);
    return ret;
}

bool serialize_OBJK_APPLICATION_Representation(MicroBuffer* buffer, const OBJK_APPLICATION_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationRefAndXML_Base(buffer, &input->base);
    return ret;
}

bool deserialize_OBJK_APPLICATION_Representation(MicroBuffer* buffer, OBJK_APPLICATION_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationRefAndXML_Base(buffer, &output->base);
    return ret;
}

bool serialize_OBJK_PUBLISHER_Representation(MicroBuffer* buffer, const OBJK_PUBLISHER_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationBinAndXML_Base(buffer, &input->base);
    ret &= serialize_ObjectId(buffer, &input->participant_id);
    return ret;
}

bool deserialize_OBJK_PUBLISHER_Representation(MicroBuffer* buffer, OBJK_PUBLISHER_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationBinAndXML_Base(buffer, &output->base);
    ret &= deserialize_ObjectId(buffer, &output->participant_id);
    return ret;
}

bool serialize_OBJK_SUBSCRIBER_Representation(MicroBuffer* buffer, const OBJK_SUBSCRIBER_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationBinAndXML_Base(buffer, &input->base);
    ret &= serialize_ObjectId(buffer, &input->participant_id);
    return ret;
}

bool deserialize_OBJK_SUBSCRIBER_Representation(MicroBuffer* buffer, OBJK_SUBSCRIBER_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationBinAndXML_Base(buffer, &output->base);
    ret &= deserialize_ObjectId(buffer, &output->participant_id);
    return ret;
}

bool serialize_DATAWRITER_Representation(MicroBuffer* buffer, const DATAWRITER_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationBinAndXML_Base(buffer, &input->base);
    ret &= serialize_ObjectId(buffer, &input->publisher_id);
    return ret;
}

bool deserialize_DATAWRITER_Representation(MicroBuffer* buffer, DATAWRITER_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationBinAndXML_Base(buffer, &output->base);
    ret &= deserialize_ObjectId(buffer, &output->publisher_id);
    return ret;
}

bool serialize_DATAREADER_Representation(MicroBuffer* buffer, const DATAREADER_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationBinAndXML_Base(buffer, &input->base);
    ret &= serialize_ObjectId(buffer, &input->subscriber_id);
    return ret;
}

bool deserialize_DATAREADER_Representation(MicroBuffer* buffer, DATAREADER_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationBinAndXML_Base(buffer, &output->base);
    ret &= deserialize_ObjectId(buffer, &output->subscriber_id);
    return ret;
}

bool serialize_OBJK_PARTICIPANT_Representation(MicroBuffer* buffer, const OBJK_PARTICIPANT_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_Representation3_Base(buffer, &input->base);
    ret &= serialize_int16_t(buffer, input->domain_id);
    return ret;
}

bool deserialize_OBJK_PARTICIPANT_Representation(MicroBuffer* buffer, OBJK_PARTICIPANT_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_Representation3_Base(buffer, &output->base);
    ret &= deserialize_int16_t(buffer, &output->domain_id);
    return ret;
}

bool serialize_OBJK_TOPIC_Representation(MicroBuffer* buffer, const OBJK_TOPIC_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_Representation3_Base(buffer, &input->base);
    ret &= serialize_ObjectId(buffer, &input->participant_id);
    return ret;
}

bool deserialize_OBJK_TOPIC_Representation(MicroBuffer* buffer, OBJK_TOPIC_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_Representation3_Base(buffer, &output->base);
    ret &= deserialize_ObjectId(buffer, &output->participant_id);
    return ret;
}

bool serialize_OBJK_DomainParticipant_Binary(MicroBuffer* buffer, const OBJK_DomainParticipant_Binary* input)
{
    bool ret = true;
    ret &= serialize_bool(buffer, input->optional_domain_reference);
    if(input->optional_domain_reference == true)
    {
        ret &= serialize_String_t(buffer, &input->domain_reference);
    }

    ret &= serialize_bool(buffer, input->optional_qos_profile_reference);
    if(input->optional_qos_profile_reference == true)
    {
        ret &= serialize_String_t(buffer, &input->qos_profile_reference);
    }

    return ret;
}

bool deserialize_OBJK_DomainParticipant_Binary(MicroBuffer* buffer, OBJK_DomainParticipant_Binary* output)
{
    bool ret = true;
    ret &= deserialize_bool(buffer, &output->optional_domain_reference);
    if(output->optional_domain_reference == true)
    {
            ret &= deserialize_String_t(buffer, &output->domain_reference);
    }

    ret &= deserialize_bool(buffer, &output->optional_qos_profile_reference);
    if(output->optional_qos_profile_reference == true)
    {
            ret &= deserialize_String_t(buffer, &output->qos_profile_reference);
    }

    return ret;
}

bool serialize_OBJK_Topic_Binary(MicroBuffer* buffer, const OBJK_Topic_Binary* input)
{
    bool ret = true;
    ret &= serialize_String_t(buffer, &input->topic_name);
    ret &= serialize_bool(buffer, input->optional_type_reference);
    if(input->optional_type_reference == true)
    {
        ret &= serialize_String_t(buffer, &input->type_reference);
    }

    ret &= serialize_bool(buffer, input->optional_type_name);
    if(input->optional_type_name == true)
    {
        ret &= serialize_String_t(buffer, &input->type_name);
    }

    return ret;
}

bool deserialize_OBJK_Topic_Binary(MicroBuffer* buffer, OBJK_Topic_Binary* output)
{
    bool ret = true;
    ret &= deserialize_String_t(buffer, &output->topic_name);
    ret &= deserialize_bool(buffer, &output->optional_type_reference);
    if(output->optional_type_reference == true)
    {
            ret &= deserialize_String_t(buffer, &output->type_reference);
    }

    ret &= deserialize_bool(buffer, &output->optional_type_name);
    if(output->optional_type_name == true)
    {
            ret &= deserialize_String_t(buffer, &output->type_name);
    }

    return ret;
}

bool serialize_OBJK_Publisher_Binary_Qos(MicroBuffer* buffer, const OBJK_Publisher_Binary_Qos* input)
{
    bool ret = true;
    ret &= serialize_bool(buffer, input->optional_partitions);
    if(input->optional_partitions == true)
    {
        ret &= serialize_StringSequence_t(buffer, &input->partitions);
    }

    ret &= serialize_bool(buffer, input->optional_group_data);
    if(input->optional_group_data == true)
    {
        ret &= serialize_BinarySequence_t(buffer, &input->group_data);
    }

    return ret;
}

bool deserialize_OBJK_Publisher_Binary_Qos(MicroBuffer* buffer, OBJK_Publisher_Binary_Qos* output)
{
    bool ret = true;
    ret &= deserialize_bool(buffer, &output->optional_partitions);
    if(output->optional_partitions == true)
    {
            ret &= deserialize_StringSequence_t(buffer, &output->partitions);
    }

    ret &= deserialize_bool(buffer, &output->optional_group_data);
    if(output->optional_group_data == true)
    {
            ret &= deserialize_BinarySequence_t(buffer, &output->group_data);
    }

    return ret;
}

bool serialize_OBJK_Publisher_Binary(MicroBuffer* buffer, const OBJK_Publisher_Binary* input)
{
    bool ret = true;
    ret &= serialize_bool(buffer, input->optional_publisher_name);
    if(input->optional_publisher_name == true)
    {
        ret &= serialize_String_t(buffer, &input->publisher_name);
    }

    ret &= serialize_bool(buffer, input->optional_qos);
    if(input->optional_qos == true)
    {
        ret &= serialize_OBJK_Publisher_Binary_Qos(buffer, &input->qos);
    }

    return ret;
}

bool deserialize_OBJK_Publisher_Binary(MicroBuffer* buffer, OBJK_Publisher_Binary* output)
{
    bool ret = true;
    ret &= deserialize_bool(buffer, &output->optional_publisher_name);
    if(output->optional_publisher_name == true)
    {
            ret &= deserialize_String_t(buffer, &output->publisher_name);
    }

    ret &= deserialize_bool(buffer, &output->optional_qos);
    if(output->optional_qos == true)
    {
            ret &= deserialize_OBJK_Publisher_Binary_Qos(buffer, &output->qos);
    }

    return ret;
}

bool serialize_OBJK_Subscriber_Binary_Qos(MicroBuffer* buffer, const OBJK_Subscriber_Binary_Qos* input)
{
    bool ret = true;
    ret &= serialize_bool(buffer, input->optional_partitions);
    if(input->optional_partitions == true)
    {
        ret &= serialize_StringSequence_t(buffer, &input->partitions);
    }

    ret &= serialize_bool(buffer, input->optional_group_data);
    if(input->optional_group_data == true)
    {
        ret &= serialize_BinarySequence_t(buffer, &input->group_data);
    }

    return ret;
}

bool deserialize_OBJK_Subscriber_Binary_Qos(MicroBuffer* buffer, OBJK_Subscriber_Binary_Qos* output)
{
    bool ret = true;
    ret &= deserialize_bool(buffer, &output->optional_partitions);
    if(output->optional_partitions == true)
    {
            ret &= deserialize_StringSequence_t(buffer, &output->partitions);
    }

    ret &= deserialize_bool(buffer, &output->optional_group_data);
    if(output->optional_group_data == true)
    {
            ret &= deserialize_BinarySequence_t(buffer, &output->group_data);
    }

    return ret;
}

bool serialize_OBJK_Subscriber_Binary(MicroBuffer* buffer, const OBJK_Subscriber_Binary* input)
{
    bool ret = true;
    ret &= serialize_bool(buffer, input->optional_subscriber_name);
    if(input->optional_subscriber_name == true)
    {
        ret &= serialize_String_t(buffer, &input->subscriber_name);
    }

    ret &= serialize_bool(buffer, input->optional_qos);
    if(input->optional_qos == true)
    {
        ret &= serialize_OBJK_Subscriber_Binary_Qos(buffer, &input->qos);
    }

    return ret;
}

bool deserialize_OBJK_Subscriber_Binary(MicroBuffer* buffer, OBJK_Subscriber_Binary* output)
{
    bool ret = true;
    ret &= deserialize_bool(buffer, &output->optional_subscriber_name);
    if(output->optional_subscriber_name == true)
    {
            ret &= deserialize_String_t(buffer, &output->subscriber_name);
    }

    ret &= deserialize_bool(buffer, &output->optional_qos);
    if(output->optional_qos == true)
    {
            ret &= deserialize_OBJK_Subscriber_Binary_Qos(buffer, &output->qos);
    }

    return ret;
}

bool serialize_OBJK_Endpoint_QosBinary(MicroBuffer* buffer, const OBJK_Endpoint_QosBinary* input)
{
    bool ret = true;
    ret &= serialize_uint16_t(buffer, input->qos_flags);
    ret &= serialize_bool(buffer, input->optional_history_depth);
    if(input->optional_history_depth == true)
    {
        ret &= serialize_uint16_t(buffer, input->history_depth);
    }

    ret &= serialize_bool(buffer, input->optional_deadline_msec);
    if(input->optional_deadline_msec == true)
    {
        ret &= serialize_uint32_t(buffer, input->deadline_msec);
    }

    ret &= serialize_bool(buffer, input->optional_lifespan_msec);
    if(input->optional_lifespan_msec == true)
    {
        ret &= serialize_uint32_t(buffer, input->lifespan_msec);
    }

    ret &= serialize_bool(buffer, input->optional_user_data);
    if(input->optional_user_data == true)
    {
        ret &= serialize_BinarySequence_t(buffer, &input->user_data);
    }

    return ret;
}

bool deserialize_OBJK_Endpoint_QosBinary(MicroBuffer* buffer, OBJK_Endpoint_QosBinary* output)
{
    bool ret = true;
    ret &= deserialize_uint16_t(buffer, &output->qos_flags);
    ret &= deserialize_bool(buffer, &output->optional_history_depth);
    if(output->optional_history_depth == true)
    {
            ret &= deserialize_uint16_t(buffer, &output->history_depth);
    }

    ret &= deserialize_bool(buffer, &output->optional_deadline_msec);
    if(output->optional_deadline_msec == true)
    {
            ret &= deserialize_uint32_t(buffer, &output->deadline_msec);
    }

    ret &= deserialize_bool(buffer, &output->optional_lifespan_msec);
    if(output->optional_lifespan_msec == true)
    {
            ret &= deserialize_uint32_t(buffer, &output->lifespan_msec);
    }

    ret &= deserialize_bool(buffer, &output->optional_user_data);
    if(output->optional_user_data == true)
    {
            ret &= deserialize_BinarySequence_t(buffer, &output->user_data);
    }

    return ret;
}

bool serialize_OBJK_DataWriter_Binary_Qos(MicroBuffer* buffer, const OBJK_DataWriter_Binary_Qos* input)
{
    bool ret = true;
    ret &= serialize_OBJK_Endpoint_QosBinary(buffer, &input->base);
    ret &= serialize_bool(buffer, input->optional_ownership_strength);
    if(input->optional_ownership_strength == true)
    {
        ret &= serialize_uint64_t(buffer, input->ownership_strength);
    }

    return ret;
}

bool deserialize_OBJK_DataWriter_Binary_Qos(MicroBuffer* buffer, OBJK_DataWriter_Binary_Qos* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_Endpoint_QosBinary(buffer, &output->base);
    ret &= deserialize_bool(buffer, &output->optional_ownership_strength);
    if(output->optional_ownership_strength == true)
    {
            ret &= deserialize_uint64_t(buffer, &output->ownership_strength);
    }

    return ret;
}

bool serialize_OBJK_DataReader_Binary_Qos(MicroBuffer* buffer, const OBJK_DataReader_Binary_Qos* input)
{
    bool ret = true;
    ret &= serialize_OBJK_Endpoint_QosBinary(buffer, &input->base);
    ret &= serialize_bool(buffer, input->optional_timebasedfilter_msec);
    if(input->optional_timebasedfilter_msec == true)
    {
        ret &= serialize_uint64_t(buffer, input->timebasedfilter_msec);
    }

    ret &= serialize_bool(buffer, input->optional_contentbased_filter);
    if(input->optional_contentbased_filter == true)
    {
        ret &= serialize_String_t(buffer, &input->contentbased_filter);
    }

    return ret;
}

bool deserialize_OBJK_DataReader_Binary_Qos(MicroBuffer* buffer, OBJK_DataReader_Binary_Qos* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_Endpoint_QosBinary(buffer, &output->base);
    ret &= deserialize_bool(buffer, &output->optional_timebasedfilter_msec);
    if(output->optional_timebasedfilter_msec == true)
    {
            ret &= deserialize_uint64_t(buffer, &output->timebasedfilter_msec);
    }

    ret &= deserialize_bool(buffer, &output->optional_contentbased_filter);
    if(output->optional_contentbased_filter == true)
    {
            ret &= deserialize_String_t(buffer, &output->contentbased_filter);
    }

    return ret;
}

bool serialize_OBJK_DataReader_Binary(MicroBuffer* buffer, const OBJK_DataReader_Binary* input)
{
    bool ret = true;
    ret &= serialize_String_t(buffer, &input->topic_name);
    ret &= serialize_bool(buffer, input->optional_qos);
    if(input->optional_qos == true)
    {
        ret &= serialize_OBJK_DataReader_Binary_Qos(buffer, &input->qos);
    }

    return ret;
}

bool deserialize_OBJK_DataReader_Binary(MicroBuffer* buffer, OBJK_DataReader_Binary* output)
{
    bool ret = true;
    ret &= deserialize_String_t(buffer, &output->topic_name);
    ret &= deserialize_bool(buffer, &output->optional_qos);
    if(output->optional_qos == true)
    {
            ret &= deserialize_OBJK_DataReader_Binary_Qos(buffer, &output->qos);
    }

    return ret;
}

bool serialize_OBJK_DataWriter_Binary(MicroBuffer* buffer, const OBJK_DataWriter_Binary* input)
{
    bool ret = true;
    ret &= serialize_String_t(buffer, &input->topic_name);
    ret &= serialize_bool(buffer, input->optional_qos);
    if(input->optional_qos == true)
    {
        ret &= serialize_OBJK_DataWriter_Binary_Qos(buffer, &input->qos);
    }

    return ret;
}

bool deserialize_OBJK_DataWriter_Binary(MicroBuffer* buffer, OBJK_DataWriter_Binary* output)
{
    bool ret = true;
    ret &= deserialize_String_t(buffer, &output->topic_name);
    ret &= deserialize_bool(buffer, &output->optional_qos);
    if(output->optional_qos == true)
    {
            ret &= deserialize_OBJK_DataWriter_Binary_Qos(buffer, &output->qos);
    }

    return ret;
}

bool serialize_ObjectVariant(MicroBuffer* buffer, const ObjectVariant* input)
{
    bool ret = true;
    ret &= serialize_uint8_t(buffer, input->kind);
    if (ret)
    {
        switch(input->kind)
        {
            case OBJK_AGENT:
                ret &= serialize_AGENT_Representation(buffer, &input->_.agent);
                break;
            case OBJK_CLIENT:
                ret &= serialize_CLIENT_Representation(buffer, &input->_.client);
                break;
            case OBJK_APPLICATION:
                ret &= serialize_OBJK_APPLICATION_Representation(buffer, &input->_.application);
                break;
            case OBJK_PARTICIPANT:
                ret &= serialize_OBJK_PARTICIPANT_Representation(buffer, &input->_.participant);
                break;
            case OBJK_QOSPROFILE:
                ret &= serialize_OBJK_QOSPROFILE_Representation(buffer, &input->_.qos_profile);
                break;
            case OBJK_TYPE:
                ret &= serialize_OBJK_TYPE_Representation(buffer, &input->_.type);
                break;
            case OBJK_TOPIC:
                ret &= serialize_OBJK_TOPIC_Representation(buffer, &input->_.topic);
                break;
            case OBJK_PUBLISHER:
                ret &= serialize_OBJK_PUBLISHER_Representation(buffer, &input->_.publisher);
                break;
            case OBJK_SUBSCRIBER:
                ret &= serialize_OBJK_SUBSCRIBER_Representation(buffer, &input->_.subscriber);
                break;
            case OBJK_DATAWRITER:
                ret &= serialize_DATAWRITER_Representation(buffer, &input->_.data_writer);
                break;
            case OBJK_DATAREADER:
                ret &= serialize_DATAREADER_Representation(buffer, &input->_.data_reader);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool deserialize_ObjectVariant(MicroBuffer* buffer, ObjectVariant* output)
{
    bool ret = true;
    ret &= deserialize_uint8_t(buffer, &output->kind);
    if (ret)
    {
        switch(output->kind)
        {
            case OBJK_AGENT:
                ret &= deserialize_AGENT_Representation(buffer, &output->_.agent);
                break;
            case OBJK_CLIENT:
                ret &= deserialize_CLIENT_Representation(buffer, &output->_.client);
                break;
            case OBJK_APPLICATION:
                ret &= deserialize_OBJK_APPLICATION_Representation(buffer, &output->_.application);
                break;
            case OBJK_PARTICIPANT:
                ret &= deserialize_OBJK_PARTICIPANT_Representation(buffer, &output->_.participant);
                break;
            case OBJK_QOSPROFILE:
                ret &= deserialize_OBJK_QOSPROFILE_Representation(buffer, &output->_.qos_profile);
                break;
            case OBJK_TYPE:
                ret &= deserialize_OBJK_TYPE_Representation(buffer, &output->_.type);
                break;
            case OBJK_TOPIC:
                ret &= deserialize_OBJK_TOPIC_Representation(buffer, &output->_.topic);
                break;
            case OBJK_PUBLISHER:
                ret &= deserialize_OBJK_PUBLISHER_Representation(buffer, &output->_.publisher);
                break;
            case OBJK_SUBSCRIBER:
                ret &= deserialize_OBJK_SUBSCRIBER_Representation(buffer, &output->_.subscriber);
                break;
            case OBJK_DATAWRITER:
                ret &= deserialize_DATAWRITER_Representation(buffer, &output->_.data_writer);
                break;
            case OBJK_DATAREADER:
                ret &= deserialize_DATAREADER_Representation(buffer, &output->_.data_reader);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool serialize_CreationMode(MicroBuffer* buffer, const CreationMode* input)
{
    bool ret = true;
    ret &= serialize_bool(buffer, input->reuse);
    ret &= serialize_bool(buffer, input->replace);
    return ret;
}

bool deserialize_CreationMode(MicroBuffer* buffer, CreationMode* output)
{
    bool ret = true;
    ret &= deserialize_bool(buffer, &output->reuse);
    ret &= deserialize_bool(buffer, &output->replace);
    return ret;
}

bool serialize_RequestId(MicroBuffer* buffer, const RequestId* input)
{
    bool ret = true;
    ret &= serialize_array_uint8_t(buffer, input->data, 2);
    return ret;
}

bool deserialize_RequestId(MicroBuffer* buffer, RequestId* output)
{
    bool ret = true;
    ret &= deserialize_array_uint8_t(buffer, output->data, 2);
    return ret;
}

bool serialize_ResultStatus(MicroBuffer* buffer, const ResultStatus* input)
{
    bool ret = true;
    ret &= serialize_uint8_t(buffer, input->status);
    ret &= serialize_uint8_t(buffer, input->implementation_status);
    return ret;
}

bool deserialize_ResultStatus(MicroBuffer* buffer, ResultStatus* output)
{
    bool ret = true;
    ret &= deserialize_uint8_t(buffer, &output->status);
    ret &= deserialize_uint8_t(buffer, &output->implementation_status);
    return ret;
}

bool serialize_BaseObjectRequest(MicroBuffer* buffer, const BaseObjectRequest* input)
{
    bool ret = true;
    ret &= serialize_RequestId(buffer, &input->request_id);
    ret &= serialize_ObjectId(buffer, &input->object_id);
    return ret;
}

bool deserialize_BaseObjectRequest(MicroBuffer* buffer, BaseObjectRequest* output)
{
    bool ret = true;
    ret &= deserialize_RequestId(buffer, &output->request_id);
    ret &= deserialize_ObjectId(buffer, &output->object_id);
    return ret;
}

bool serialize_AGENT_ActivityInfo(MicroBuffer* buffer, const AGENT_ActivityInfo* input)
{
    bool ret = true;
    ret &= serialize_int16_t(buffer, input->availibility);
    ret &= serialize_TransportLocatorSeq(buffer, &input->address_seq);
    return ret;
}

bool deserialize_AGENT_ActivityInfo(MicroBuffer* buffer, AGENT_ActivityInfo* output)
{
    bool ret = true;
    ret &= deserialize_int16_t(buffer, &output->availibility);
    ret &= deserialize_TransportLocatorSeq(buffer, &output->address_seq);
    return ret;
}

bool serialize_DATAREADER_ActivityInfo(MicroBuffer* buffer, const DATAREADER_ActivityInfo* input)
{
    bool ret = true;
    ret &= serialize_int16_t(buffer, input->highest_acked_num);
    return ret;
}

bool deserialize_DATAREADER_ActivityInfo(MicroBuffer* buffer, DATAREADER_ActivityInfo* output)
{
    bool ret = true;
    ret &= deserialize_int16_t(buffer, &output->highest_acked_num);
    return ret;
}

bool serialize_DATAWRITER_ActivityInfo(MicroBuffer* buffer, const DATAWRITER_ActivityInfo* input)
{
    bool ret = true;
    ret &= serialize_int16_t(buffer, input->stream_seq_num);
    ret &= serialize_uint64_t(buffer, input->sample_seq_num);
    return ret;
}

bool deserialize_DATAWRITER_ActivityInfo(MicroBuffer* buffer, DATAWRITER_ActivityInfo* output)
{
    bool ret = true;
    ret &= deserialize_int16_t(buffer, &output->stream_seq_num);
    ret &= deserialize_uint64_t(buffer, &output->sample_seq_num);
    return ret;
}

bool serialize_ActivityInfoVariant(MicroBuffer* buffer, const ActivityInfoVariant* input)
{
    bool ret = true;
    ret &= serialize_uint8_t(buffer, input->kind);
    if (ret)
    {
        switch(input->kind)
        {
            case OBJK_AGENT:
                ret &= serialize_AGENT_ActivityInfo(buffer, &input->_.agent);
                break;
            case OBJK_DATAWRITER:
                ret &= serialize_DATAWRITER_ActivityInfo(buffer, &input->_.data_writer);
                break;
            case OBJK_DATAREADER:
                ret &= serialize_DATAREADER_ActivityInfo(buffer, &input->_.data_reader);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool deserialize_ActivityInfoVariant(MicroBuffer* buffer, ActivityInfoVariant* output)
{
    bool ret = true;
    ret &= deserialize_uint8_t(buffer, &output->kind);
    if (ret)
    {
        switch(output->kind)
        {
            case OBJK_AGENT:
                ret &= deserialize_AGENT_ActivityInfo(buffer, &output->_.agent);
                break;
            case OBJK_DATAWRITER:
                ret &= deserialize_DATAWRITER_ActivityInfo(buffer, &output->_.data_writer);
                break;
            case OBJK_DATAREADER:
                ret &= deserialize_DATAREADER_ActivityInfo(buffer, &output->_.data_reader);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool serialize_ObjectInfo(MicroBuffer* buffer, const ObjectInfo* input)
{
    bool ret = true;
    ret &= serialize_bool(buffer, input->optional_config);
    if(input->optional_config == true)
    {
        ret &= serialize_ObjectVariant(buffer, &input->config);
    }

    ret &= serialize_bool(buffer, input->optional_activity);
    if(input->optional_activity == true)
    {
        ret &= serialize_ActivityInfoVariant(buffer, &input->activity);
    }

    return ret;
}

bool deserialize_ObjectInfo(MicroBuffer* buffer, ObjectInfo* output)
{
    bool ret = true;
    ret &= deserialize_bool(buffer, &output->optional_config);
    if(output->optional_config == true)
    {
            ret &= deserialize_ObjectVariant(buffer, &output->config);
    }

    ret &= deserialize_bool(buffer, &output->optional_activity);
    if(output->optional_activity == true)
    {
            ret &= deserialize_ActivityInfoVariant(buffer, &output->activity);
    }

    return ret;
}

bool serialize_BaseObjectReply(MicroBuffer* buffer, const BaseObjectReply* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->related_request);
    ret &= serialize_ResultStatus(buffer, &input->result);
    return ret;
}

bool deserialize_BaseObjectReply(MicroBuffer* buffer, BaseObjectReply* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->related_request);
    ret &= deserialize_ResultStatus(buffer, &output->result);
    return ret;
}

bool serialize_DataDeliveryControl(MicroBuffer* buffer, const DataDeliveryControl* input)
{
    bool ret = true;
    ret &= serialize_uint16_t(buffer, input->max_samples);
    ret &= serialize_uint16_t(buffer, input->max_elapsed_time);
    ret &= serialize_uint16_t(buffer, input->max_bytes_per_seconds);
    ret &= serialize_uint16_t(buffer, input->min_pace_period);
    return ret;
}

bool deserialize_DataDeliveryControl(MicroBuffer* buffer, DataDeliveryControl* output)
{
    bool ret = true;
    ret &= deserialize_uint16_t(buffer, &output->max_samples);
    ret &= deserialize_uint16_t(buffer, &output->max_elapsed_time);
    ret &= deserialize_uint16_t(buffer, &output->max_bytes_per_seconds);
    ret &= deserialize_uint16_t(buffer, &output->min_pace_period);
    return ret;
}

bool serialize_ReadSpecification(MicroBuffer* buffer, const ReadSpecification* input)
{
    bool ret = true;
    ret &= serialize_uint8_t(buffer, input->data_format);
    ret &= serialize_bool(buffer, input->optional_content_filter_expression);
    if(input->optional_content_filter_expression == true)
    {
        ret &= serialize_String_t(buffer, &input->content_filter_expression);
    }

    ret &= serialize_bool(buffer, input->optional_delivery_control);
    if(input->optional_delivery_control == true)
    {
        ret &= serialize_DataDeliveryControl(buffer, &input->delivery_control);
    }

    return ret;
}

bool deserialize_ReadSpecification(MicroBuffer* buffer, ReadSpecification* output)
{
    bool ret = true;
    ret &= deserialize_uint8_t(buffer, &output->data_format);
    ret &= deserialize_bool(buffer, &output->optional_content_filter_expression);
    if(output->optional_content_filter_expression == true)
    {
            ret &= deserialize_String_t(buffer, &output->content_filter_expression);
    }

    ret &= deserialize_bool(buffer, &output->optional_delivery_control);
    if(output->optional_delivery_control == true)
    {
            ret &= deserialize_DataDeliveryControl(buffer, &output->delivery_control);
    }

    return ret;
}

bool serialize_SeqNumberAndTimestamp(MicroBuffer* buffer, const SeqNumberAndTimestamp* input)
{
    bool ret = true;
    ret &= serialize_uint32_t(buffer, input->sequence_number);
    ret &= serialize_uint32_t(buffer, input->session_time_offset);
    return ret;
}

bool deserialize_SeqNumberAndTimestamp(MicroBuffer* buffer, SeqNumberAndTimestamp* output)
{
    bool ret = true;
    ret &= deserialize_uint32_t(buffer, &output->sequence_number);
    ret &= deserialize_uint32_t(buffer, &output->session_time_offset);
    return ret;
}

bool serialize_SampleInfoDetail(MicroBuffer* buffer, const SampleInfoDetail* input)
{
    bool ret = true;
    ret &= serialize_uint32_t(buffer, input->format);
    if (ret)
    {
        switch(input->format)
        {
            case FORMAT_EMPTY:
                ret &= serialize_uint32_t(buffer, input->_.sequence_number);
                break;
            case FORMAT_SEQNUM:
                ret &= serialize_uint32_t(buffer, input->_.session_time_offset);
                break;
            case FORMAT_TIMESTAMP:
                ret &= serialize_SeqNumberAndTimestamp(buffer, &input->_.seqnum_n_timestamp);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool deserialize_SampleInfoDetail(MicroBuffer* buffer, SampleInfoDetail* output)
{
    bool ret = true;
    ret &= deserialize_uint32_t(buffer, &output->format);
    if (ret)
    {
        switch(output->format)
        {
            case FORMAT_EMPTY:
                ret &= deserialize_uint32_t(buffer, &output->_.sequence_number);
                break;
            case FORMAT_SEQNUM:
                ret &= deserialize_uint32_t(buffer, &output->_.session_time_offset);
                break;
            case FORMAT_TIMESTAMP:
                ret &= deserialize_SeqNumberAndTimestamp(buffer, &output->_.seqnum_n_timestamp);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool serialize_SampleInfo(MicroBuffer* buffer, const SampleInfo* input)
{
    bool ret = true;
    ret &= serialize_uint8_t(buffer, input->state);
    ret &= serialize_SampleInfoDetail(buffer, &input->detail);
    return ret;
}

bool deserialize_SampleInfo(MicroBuffer* buffer, SampleInfo* output)
{
    bool ret = true;
    ret &= deserialize_uint8_t(buffer, &output->state);
    ret &= deserialize_SampleInfoDetail(buffer, &output->detail);
    return ret;
}

bool serialize_SampleInfoDelta(MicroBuffer* buffer, const SampleInfoDelta* input)
{
    bool ret = true;
    ret &= serialize_uint8_t(buffer, input->state);
    ret &= serialize_uint8_t(buffer, input->seq_number_delta);
    ret &= serialize_uint16_t(buffer, input->timestamp_delta);
    return ret;
}

bool deserialize_SampleInfoDelta(MicroBuffer* buffer, SampleInfoDelta* output)
{
    bool ret = true;
    ret &= deserialize_uint8_t(buffer, &output->state);
    ret &= deserialize_uint8_t(buffer, &output->seq_number_delta);
    ret &= deserialize_uint16_t(buffer, &output->timestamp_delta);
    return ret;
}

bool serialize_SampleData(MicroBuffer* buffer, const SampleData* input)
{
    return serialize_sequence_uint8_t(buffer, input->data, input->size);
}

bool deserialize_SampleData(MicroBuffer* buffer, SampleData* output)
{
    return deserialize_sequence_uint8_t(buffer, &output->data, &output->size);
}

bool serialize_SampleDataSeq(MicroBuffer* buffer, const SampleDataSeq* input)
{
    bool ret = serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size && ret; i++)
    {
        ret = serialize_SampleData(buffer, &input->data[i]);
    }
    return ret;
}

bool deserialize_SampleDataSeq(MicroBuffer* buffer, SampleDataSeq* output)
{
    bool ret = deserialize_uint32_t(buffer, &output->size);
    for(uint32_t i = 0; i < output->size && ret; i++)
    {
        ret = deserialize_SampleData(buffer, &output->data[i]);
    }
    return ret;
}

bool serialize_Sample(MicroBuffer* buffer, const Sample* input)
{
    bool ret = true;
    ret &= serialize_SampleInfo(buffer, &input->info);
    ret &= serialize_SampleData(buffer, &input->data);
    return ret;
}

bool deserialize_Sample(MicroBuffer* buffer, Sample* output)
{
    bool ret = true;
    ret &= deserialize_SampleInfo(buffer, &output->info);
    ret &= deserialize_SampleData(buffer, &output->data);
    return ret;
}

bool serialize_SampleSeq(MicroBuffer* buffer, const SampleSeq* input)
{
    bool ret = serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size && ret; i++)
    {
        ret = serialize_Sample(buffer, &input->data[i]);
    }
    return ret;
}

bool deserialize_SampleSeq(MicroBuffer* buffer, SampleSeq* output)
{
    bool ret = deserialize_uint32_t(buffer, &output->size);
    for(uint32_t i = 0; i < output->size && ret; i++)
    {
        ret = deserialize_Sample(buffer, &output->data[i]);
    }
    return ret;
}

bool serialize_SampleDelta(MicroBuffer* buffer, const SampleDelta* input)
{
    bool ret = true;
    ret &= serialize_SampleInfoDelta(buffer, &input->info_delta);
    ret &= serialize_SampleData(buffer, &input->data);
    return ret;
}

bool deserialize_SampleDelta(MicroBuffer* buffer, SampleDelta* output)
{
    bool ret = true;
    ret &= deserialize_SampleInfoDelta(buffer, &output->info_delta);
    ret &= deserialize_SampleData(buffer, &output->data);
    return ret;
}

bool serialize_SampleDeltaSequence(MicroBuffer* buffer, const SampleDeltaSequence* input)
{
    bool ret = serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size && ret; i++)
    {
        ret = serialize_SampleDelta(buffer, &input->data[i]);
    }
    return ret;
}

bool deserialize_SampleDeltaSequence(MicroBuffer* buffer, SampleDeltaSequence* output)
{
    bool ret = deserialize_uint32_t(buffer, &output->size);
    for(uint32_t i = 0; i < output->size && ret; i++)
    {
        ret = deserialize_SampleDelta(buffer, &output->data[i]);
    }
    return ret;
}

bool serialize_PackedSamples(MicroBuffer* buffer, const PackedSamples* input)
{
    bool ret = true;
    ret &= serialize_SampleInfo(buffer, &input->info_base);
    ret &= serialize_SampleDeltaSequence(buffer, &input->sample_delta_seq);
    return ret;
}

bool deserialize_PackedSamples(MicroBuffer* buffer, PackedSamples* output)
{
    bool ret = true;
    ret &= deserialize_SampleInfo(buffer, &output->info_base);
    ret &= deserialize_SampleDeltaSequence(buffer, &output->sample_delta_seq);
    return ret;
}

bool serialize_SamplePackedSeq(MicroBuffer* buffer, const SamplePackedSeq* input)
{
    bool ret = serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size && ret; i++)
    {
        ret = serialize_PackedSamples(buffer, &input->data[i]);
    }
    return ret;
}

bool deserialize_SamplePackedSeq(MicroBuffer* buffer, SamplePackedSeq* output)
{
    bool ret = deserialize_uint32_t(buffer, &output->size);
    for(uint32_t i = 0; i < output->size && ret; i++)
    {
        ret = deserialize_PackedSamples(buffer, &output->data[i]);
    }
    return ret;
}

bool serialize_DataRepresentation(MicroBuffer* buffer, const DataRepresentation* input)
{
    bool ret = true;
    ret &= serialize_uint8_t(buffer, input->format);
    if (ret)
    {
        switch(input->format)
        {
            case FORMAT_DATA:
                ret &= serialize_SampleData(buffer, &input->_.data);
                break;
            case FORMAT_SAMPLE:
                ret &= serialize_Sample(buffer, &input->_.sample);
                break;
            case FORMAT_DATA_SEQ:
                ret &= serialize_SampleDataSeq(buffer, &input->_.data_seq);
                break;
            case FORMAT_SAMPLE_SEQ:
                ret &= serialize_SampleSeq(buffer, &input->_.sample_seq);
                break;
            case FORMAT_PACKED_SAMPLES:
                ret &= serialize_PackedSamples(buffer, &input->_.packed_samples);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool deserialize_DataRepresentation(MicroBuffer* buffer, DataRepresentation* output)
{
    bool ret = true;
    ret &= deserialize_uint8_t(buffer, &output->format);
    if (ret)
    {
        switch(output->format)
        {
            case FORMAT_DATA:
                ret &= deserialize_SampleData(buffer, &output->_.data);
                break;
            case FORMAT_SAMPLE:
                ret &= deserialize_Sample(buffer, &output->_.sample);
                break;
            case FORMAT_DATA_SEQ:
                ret &= deserialize_SampleDataSeq(buffer, &output->_.data_seq);
                break;
            case FORMAT_SAMPLE_SEQ:
                ret &= deserialize_SampleSeq(buffer, &output->_.sample_seq);
                break;
            case FORMAT_PACKED_SAMPLES:
                ret &= deserialize_PackedSamples(buffer, &output->_.packed_samples);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool serialize_CREATE_CLIENT_Payload(MicroBuffer* buffer, const CREATE_CLIENT_Payload* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_CLIENT_Representation(buffer, &input->client_representation);
    return ret;
}

bool deserialize_CREATE_CLIENT_Payload(MicroBuffer* buffer, CREATE_CLIENT_Payload* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_CLIENT_Representation(buffer, &output->client_representation);
    return ret;
}

bool serialize_CREATE_Payload(MicroBuffer* buffer, const CREATE_Payload* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_ObjectVariant(buffer, &input->object_representation);
    return ret;
}

bool deserialize_CREATE_Payload(MicroBuffer* buffer, CREATE_Payload* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_ObjectVariant(buffer, &output->object_representation);
    return ret;
}

bool serialize_GET_INFO_Payload(MicroBuffer* buffer, const GET_INFO_Payload* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_uint32_t(buffer, input->info_mask);
    return ret;
}

bool deserialize_GET_INFO_Payload(MicroBuffer* buffer, GET_INFO_Payload* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_uint32_t(buffer, &output->info_mask);
    return ret;
}

bool serialize_DELETE_Payload(MicroBuffer* buffer, const DELETE_Payload* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    return ret;
}

bool deserialize_DELETE_Payload(MicroBuffer* buffer, DELETE_Payload* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    return ret;
}

bool serialize_STATUS_AGENT_Payload(MicroBuffer* buffer, const STATUS_AGENT_Payload* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectReply(buffer, &input->base);
    ret &= serialize_AGENT_Representation(buffer, &input->agent_info);
    return ret;
}

bool deserialize_STATUS_AGENT_Payload(MicroBuffer* buffer, STATUS_AGENT_Payload* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectReply(buffer, &output->base);
    ret &= deserialize_AGENT_Representation(buffer, &output->agent_info);
    return ret;
}

bool serialize_STATUS_Payload(MicroBuffer* buffer, const STATUS_Payload* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectReply(buffer, &input->base);
    return ret;
}

bool deserialize_STATUS_Payload(MicroBuffer* buffer, STATUS_Payload* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectReply(buffer, &output->base);
    return ret;
}

bool serialize_INFO_Payload(MicroBuffer* buffer, const INFO_Payload* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectReply(buffer, &input->base);
    ret &= serialize_ObjectInfo(buffer, &input->object_info);
    return ret;
}

bool deserialize_INFO_Payload(MicroBuffer* buffer, INFO_Payload* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectReply(buffer, &output->base);
    ret &= deserialize_ObjectInfo(buffer, &output->object_info);
    return ret;
}

bool serialize_READ_DATA_Payload(MicroBuffer* buffer, const READ_DATA_Payload* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_ReadSpecification(buffer, &input->read_specification);
    return ret;
}

bool deserialize_READ_DATA_Payload(MicroBuffer* buffer, READ_DATA_Payload* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_ReadSpecification(buffer, &output->read_specification);
    return ret;
}

bool serialize_WRITE_DATA_Payload_Data(MicroBuffer* buffer, const WRITE_DATA_Payload_Data* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_SampleData(buffer, &input->data);
    return ret;
}

bool deserialize_WRITE_DATA_Payload_Data(MicroBuffer* buffer, WRITE_DATA_Payload_Data* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_SampleData(buffer, &output->data);
    return ret;
}

bool serialize_WRITE_DATA_Payload_Sample(MicroBuffer* buffer, const WRITE_DATA_Payload_Sample* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_Sample(buffer, &input->sample);
    return ret;
}

bool deserialize_WRITE_DATA_Payload_Sample(MicroBuffer* buffer, WRITE_DATA_Payload_Sample* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_Sample(buffer, &output->sample);
    return ret;
}

bool serialize_WRITE_DATA_Payload_DataSeq(MicroBuffer* buffer, const WRITE_DATA_Payload_DataSeq* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_SampleDataSeq(buffer, &input->data_seq);
    return ret;
}

bool deserialize_WRITE_DATA_Payload_DataSeq(MicroBuffer* buffer, WRITE_DATA_Payload_DataSeq* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_SampleDataSeq(buffer, &output->data_seq);
    return ret;
}

bool serialize_WRITE_DATA_Payload_SampleSeq(MicroBuffer* buffer, const WRITE_DATA_Payload_SampleSeq* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_SampleSeq(buffer, &input->sample_seq);
    return ret;
}

bool deserialize_WRITE_DATA_Payload_SampleSeq(MicroBuffer* buffer, WRITE_DATA_Payload_SampleSeq* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_SampleSeq(buffer, &output->sample_seq);
    return ret;
}

bool serialize_WRITE_DATA_Payload_PackedSamples(MicroBuffer* buffer, const WRITE_DATA_Payload_PackedSamples* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_PackedSamples(buffer, &input->packed_samples);
    return ret;
}

bool deserialize_WRITE_DATA_Payload_PackedSamples(MicroBuffer* buffer, WRITE_DATA_Payload_PackedSamples* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_PackedSamples(buffer, &output->packed_samples);
    return ret;
}

bool serialize_DATA_Payload_Data(MicroBuffer* buffer, const DATA_Payload_Data* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_SampleData(buffer, &input->data);
    return ret;
}

bool deserialize_DATA_Payload_Data(MicroBuffer* buffer, DATA_Payload_Data* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_SampleData(buffer, &output->data);
    return ret;
}

bool serialize_DATA_Payload_Sample(MicroBuffer* buffer, const DATA_Payload_Sample* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_Sample(buffer, &input->sample);
    return ret;
}

bool deserialize_DATA_Payload_Sample(MicroBuffer* buffer, DATA_Payload_Sample* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_Sample(buffer, &output->sample);
    return ret;
}

bool serialize_DATA_Payload_DataSeq(MicroBuffer* buffer, const DATA_Payload_DataSeq* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_SampleDataSeq(buffer, &input->data_seq);
    return ret;
}

bool deserialize_DATA_Payload_DataSeq(MicroBuffer* buffer, DATA_Payload_DataSeq* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_SampleDataSeq(buffer, &output->data_seq);
    return ret;
}

bool serialize_DATA_Payload_SampleSeq(MicroBuffer* buffer, const DATA_Payload_SampleSeq* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_SampleSeq(buffer, &input->sample_seq);
    return ret;
}

bool deserialize_DATA_Payload_SampleSeq(MicroBuffer* buffer, DATA_Payload_SampleSeq* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_SampleSeq(buffer, &output->sample_seq);
    return ret;
}

bool serialize_DATA_Payload_PackedSamples(MicroBuffer* buffer, const DATA_Payload_PackedSamples* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_PackedSamples(buffer, &input->packed_samples);
    return ret;
}

bool deserialize_DATA_Payload_PackedSamples(MicroBuffer* buffer, DATA_Payload_PackedSamples* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_PackedSamples(buffer, &output->packed_samples);
    return ret;
}

bool serialize_ACKNACK_Payload(MicroBuffer* buffer, const ACKNACK_Payload* input)
{
    bool ret = true;
    ret &= serialize_int16_t(buffer, input->first_unacked_seq_num);
    ret &= serialize_array_uint8_t(buffer, input->nack_bitmap, 2);
    return ret;
}

bool deserialize_ACKNACK_Payload(MicroBuffer* buffer, ACKNACK_Payload* output)
{
    bool ret = true;
    ret &= deserialize_int16_t(buffer, &output->first_unacked_seq_num);
    ret &= deserialize_array_uint8_t(buffer, output->nack_bitmap, 2);
    return ret;
}

bool serialize_HEARTBEAT_Payload(MicroBuffer* buffer, const HEARTBEAT_Payload* input)
{
    bool ret = true;
    ret &= serialize_int16_t(buffer, input->first_unacked_seq_nr);
    ret &= serialize_int16_t(buffer, input->last_unacked_seq_nr);
    return ret;
}

bool deserialize_HEARTBEAT_Payload(MicroBuffer* buffer, HEARTBEAT_Payload* output)
{
    bool ret = true;
    ret &= deserialize_int16_t(buffer, &output->first_unacked_seq_nr);
    ret &= deserialize_int16_t(buffer, &output->last_unacked_seq_nr);
    return ret;
}

