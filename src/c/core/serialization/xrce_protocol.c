#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <string.h>

//==================================================================
//                             PUBLIC
//==================================================================
bool serialize_Time_t(mcBuffer* buffer, const Time_t* input)
{
    bool ret = true;
    ret &= mc_serialize_int32_t(buffer, input->seconds);
    ret &= mc_serialize_uint32_t(buffer, input->nanoseconds);
    return ret;
}

bool deserialize_Time_t(mcBuffer* buffer, Time_t* output)
{
    bool ret = true;
    ret &= mc_deserialize_int32_t(buffer, &output->seconds);
    ret &= mc_deserialize_uint32_t(buffer, &output->nanoseconds);
    return ret;
}

bool serialize_BinarySequence_t(mcBuffer* buffer, const BinarySequence_t* input)
{
    return mc_serialize_sequence_uint8_t(buffer, input->data, input->size);
}

bool deserialize_BinarySequence_t(mcBuffer* buffer, BinarySequence_t* output)
{
    return mc_deserialize_sequence_uint8_t(buffer, output->data, BINARY_SEQUENCE_MAX, &output->size);
}

bool serialize_StringSequence_t(mcBuffer* buffer, const StringSequence_t* input)
{
    bool ret = mc_serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size && ret; i++)
    {
        ret = mc_serialize_string(buffer, input->data[i]);
    }
    return ret;
}

bool deserialize_StringSequence_t(mcBuffer* buffer, StringSequence_t* output)
{
    bool ret = mc_deserialize_uint32_t(buffer, &output->size);
    if(output->size > STRING_SEQUENCE_MAX)
    {
        buffer->error = true;
        ret = false;
    }
    else
    {
        for(uint32_t i = 0; i < output->size && ret; i++)
        {
            ret = mc_deserialize_string(buffer, output->data[i], STRING_SIZE_MAX);
        }
    }
    return ret;
}

bool serialize_ClientKey(mcBuffer* buffer, const ClientKey* input)
{
    bool ret = true;
    ret &= mc_serialize_array_uint8_t(buffer, input->data, 4);
    return ret;
}

bool deserialize_ClientKey(mcBuffer* buffer, ClientKey* output)
{
    bool ret = true;
    ret &= mc_deserialize_array_uint8_t(buffer, output->data, 4);
    return ret;
}

bool serialize_ObjectId(mcBuffer* buffer, const ObjectId* input)
{
    bool ret = true;
    ret &= mc_serialize_array_uint8_t(buffer, input->data, 2);
    return ret;
}

bool deserialize_ObjectId(mcBuffer* buffer, ObjectId* output)
{
    bool ret = true;
    ret &= mc_deserialize_array_uint8_t(buffer, output->data, 2);
    return ret;
}

bool serialize_ObjectPrefix(mcBuffer* buffer, const ObjectPrefix* input)
{
    bool ret = true;
    ret &= mc_serialize_array_uint8_t(buffer, input->data, 2);
    return ret;
}

bool deserialize_ObjectPrefix(mcBuffer* buffer, ObjectPrefix* output)
{
    bool ret = true;
    ret &= mc_deserialize_array_uint8_t(buffer, output->data, 2);
    return ret;
}

bool serialize_XrceCookie(mcBuffer* buffer, const XrceCookie* input)
{
    bool ret = true;
    ret &= mc_serialize_array_uint8_t(buffer, input->data, 4);
    return ret;
}

bool deserialize_XrceCookie(mcBuffer* buffer, XrceCookie* output)
{
    bool ret = true;
    ret &= mc_deserialize_array_uint8_t(buffer, output->data, 4);
    return ret;
}

bool serialize_XrceVersion(mcBuffer* buffer, const XrceVersion* input)
{
    bool ret = true;
    ret &= mc_serialize_array_uint8_t(buffer, input->data, 2);
    return ret;
}

bool deserialize_XrceVersion(mcBuffer* buffer, XrceVersion* output)
{
    bool ret = true;
    ret &= mc_deserialize_array_uint8_t(buffer, output->data, 2);
    return ret;
}

bool serialize_XrceVendorId(mcBuffer* buffer, const XrceVendorId* input)
{
    bool ret = true;
    ret &= mc_serialize_array_uint8_t(buffer, input->data, 2);
    return ret;
}

bool deserialize_XrceVendorId(mcBuffer* buffer, XrceVendorId* output)
{
    bool ret = true;
    ret &= mc_deserialize_array_uint8_t(buffer, output->data, 2);
    return ret;
}

bool serialize_TransportLocatorSmall(mcBuffer* buffer, const TransportLocatorSmall* input)
{
    bool ret = true;
    ret &= mc_serialize_array_uint8_t(buffer, input->address, 2);
    ret &= mc_serialize_uint8_t(buffer, input->locator_port);
    return ret;
}

bool deserialize_TransportLocatorSmall(mcBuffer* buffer, TransportLocatorSmall* output)
{
    bool ret = true;
    ret &= mc_deserialize_array_uint8_t(buffer, output->address, 2);
    ret &= mc_deserialize_uint8_t(buffer, &output->locator_port);
    return ret;
}

bool serialize_TransportLocatorMedium(mcBuffer* buffer, const TransportLocatorMedium* input)
{
    bool ret = true;
    ret &= mc_serialize_array_uint8_t(buffer, input->address, 4);
    ret &= mc_serialize_uint8_t(buffer, input->locator_port);
    return ret;
}

bool deserialize_TransportLocatorMedium(mcBuffer* buffer, TransportLocatorMedium* output)
{
    bool ret = true;
    ret &= mc_deserialize_array_uint8_t(buffer, output->address, 4);
    ret &= mc_deserialize_uint8_t(buffer, &output->locator_port);
    return ret;
}

bool serialize_TransportLocatorLarge(mcBuffer* buffer, const TransportLocatorLarge* input)
{
    bool ret = true;
    ret &= mc_serialize_array_uint8_t(buffer, input->address, 16);
    ret &= mc_serialize_uint8_t(buffer, input->locator_port);
    return ret;
}

bool deserialize_TransportLocatorLarge(mcBuffer* buffer, TransportLocatorLarge* output)
{
    bool ret = true;
    ret &= mc_deserialize_array_uint8_t(buffer, output->address, 16);
    ret &= mc_deserialize_uint8_t(buffer, &output->locator_port);
    return ret;
}

bool serialize_TransportLocatorString(mcBuffer* buffer, const TransportLocatorString* input)
{
    bool ret = true;
    ret &= mc_serialize_string(buffer, input->value);
    return ret;
}

bool deserialize_TransportLocatorString(mcBuffer* buffer, TransportLocatorString* output)
{
    bool ret = true;
    ret &= mc_deserialize_string(buffer, output->value, STRING_SIZE_MAX);
    return ret;
}

bool serialize_TransportLocator(mcBuffer* buffer, const TransportLocator* input)
{
    bool ret = true;
    ret &= mc_serialize_uint8_t(buffer, input->format);
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

bool deserialize_TransportLocator(mcBuffer* buffer, TransportLocator* output)
{
    bool ret = true;
    ret &= mc_deserialize_uint8_t(buffer, &output->format);
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

bool serialize_TransportLocatorSeq(mcBuffer* buffer, const TransportLocatorSeq* input)
{
    bool ret = mc_serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size && ret; i++)
    {
        ret = serialize_TransportLocator(buffer, &input->data[i]);
    }
    return ret;
}

bool deserialize_TransportLocatorSeq(mcBuffer* buffer, TransportLocatorSeq* output)
{
    bool ret = mc_deserialize_uint32_t(buffer, &output->size);
    if(output->size > TRANSPORT_LOCATOR_SEQUENCE_MAX)
    {
        buffer->error = true;
        ret = false;
    }
    else
    {
        for(uint32_t i = 0; i < output->size && ret; i++)
        {
            ret = deserialize_TransportLocator(buffer, &output->data[i]);
        }
    }
    return ret;
}

bool serialize_Property(mcBuffer* buffer, const Property* input)
{
    bool ret = true;
    ret &= mc_serialize_string(buffer, input->name);
    ret &= mc_serialize_string(buffer, input->value);
    return ret;
}

bool deserialize_Property(mcBuffer* buffer, Property* output)
{
    bool ret = true;
    ret &= mc_deserialize_string(buffer, output->name, STRING_SIZE_MAX);
    ret &= mc_deserialize_string(buffer, output->value, STRING_SIZE_MAX);
    return ret;
}

bool serialize_PropertySeq(mcBuffer* buffer, const PropertySeq* input)
{
    bool ret = mc_serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size && ret; i++)
    {
        ret = serialize_Property(buffer, input->data + i);
    }
    return ret;
}

bool deserialize_PropertySeq(mcBuffer* buffer, PropertySeq* output)
{
    bool ret = mc_deserialize_uint32_t(buffer, &output->size);

    if(output->size > PROPERTY_SEQUENCE_MAX)
    {
        buffer->error = true;
        ret = false;
    }
    else
    {
        for(uint32_t i = 0; i < output->size && ret; i++)
        {
            ret = deserialize_Property(buffer, &output->data[i]);
        }
    }
    return ret;
}

bool serialize_CLIENT_Representation(mcBuffer* buffer, const CLIENT_Representation* input)
{
    bool ret = true;
    ret &= serialize_XrceCookie(buffer, &input->xrce_cookie);
    ret &= serialize_XrceVersion(buffer, &input->xrce_version);
    ret &= serialize_XrceVendorId(buffer, &input->xrce_vendor_id);
    ret &= serialize_Time_t(buffer, &input->client_timestamp);
    ret &= serialize_ClientKey(buffer, &input->client_key);
    ret &= mc_serialize_uint8_t(buffer, input->session_id);
    ret &= mc_serialize_bool(buffer, input->optional_properties);
    if(input->optional_properties == true)
    {
        ret &= serialize_PropertySeq(buffer, &input->properties);
    }

    return ret;
}

bool deserialize_CLIENT_Representation(mcBuffer* buffer, CLIENT_Representation* output)
{
    bool ret = true;
    ret &= deserialize_XrceCookie(buffer, &output->xrce_cookie);
    ret &= deserialize_XrceVersion(buffer, &output->xrce_version);
    ret &= deserialize_XrceVendorId(buffer, &output->xrce_vendor_id);
    ret &= deserialize_Time_t(buffer, &output->client_timestamp);
    ret &= deserialize_ClientKey(buffer, &output->client_key);
    ret &= mc_deserialize_uint8_t(buffer, &output->session_id);
    ret &= mc_deserialize_bool(buffer, &output->optional_properties);
    if(output->optional_properties == true)
    {
            ret &= deserialize_PropertySeq(buffer, &output->properties);
    }

    return ret;
}

bool serialize_AGENT_Representation(mcBuffer* buffer, const AGENT_Representation* input)
{
    bool ret = true;
    ret &= serialize_XrceCookie(buffer, &input->xrce_cookie);
    ret &= serialize_XrceVersion(buffer, &input->xrce_version);
    ret &= serialize_XrceVendorId(buffer, &input->xrce_vendor_id);
    ret &= serialize_Time_t(buffer, &input->agent_timestamp);
    ret &= mc_serialize_bool(buffer, input->optional_properties);
    if(input->optional_properties == true)
    {
        ret &= serialize_PropertySeq(buffer, &input->properties);
    }

    return ret;
}

bool deserialize_AGENT_Representation(mcBuffer* buffer, AGENT_Representation* output)
{
    bool ret = true;
    ret &= deserialize_XrceCookie(buffer, &output->xrce_cookie);
    ret &= deserialize_XrceVersion(buffer, &output->xrce_version);
    ret &= deserialize_XrceVendorId(buffer, &output->xrce_vendor_id);
    ret &= deserialize_Time_t(buffer, &output->agent_timestamp);
    ret &= mc_deserialize_bool(buffer, &output->optional_properties);
    // TODO (julian): modified with the new Micro ROS API.
//    if(output->optional_properties == true)
//    {
//            ret &= deserialize_PropertySeq(buffer, &output->properties);
//    }

    return ret;
}

bool serialize_OBJK_Representation3Formats(mcBuffer* buffer, const OBJK_Representation3Formats* input)
{
    bool ret = true;
    ret &= mc_serialize_uint8_t(buffer, input->format);
    if (ret)
    {
        switch(input->format)
        {
            case REPRESENTATION_BY_REFERENCE:
                ret &= mc_serialize_string(buffer, input->_.object_reference);
                break;
            case REPRESENTATION_AS_XML_STRING:
                ret &= mc_serialize_string(buffer, input->_.xml_string_represenatation);
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

bool deserialize_OBJK_Representation3Formats(mcBuffer* buffer, OBJK_Representation3Formats* output)
{
    bool ret = true;
    ret &= mc_deserialize_uint8_t(buffer, &output->format);
    if (ret)
    {
        switch(output->format)
        {
            case REPRESENTATION_BY_REFERENCE:
                ret &= mc_deserialize_string(buffer, output->_.object_reference, STRING_SIZE_MAX);
                break;
            case REPRESENTATION_AS_XML_STRING:
                ret &= mc_deserialize_string(buffer, output->_.xml_string_represenatation, STRING_SIZE_MAX);
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

bool serialize_OBJK_RepresentationRefAndXMLFormats(mcBuffer* buffer, const OBJK_RepresentationRefAndXMLFormats* input)
{
    bool ret = true;
    ret &= mc_serialize_uint8_t(buffer, input->format);
    if (ret)
    {
        switch(input->format)
        {
            case REPRESENTATION_BY_REFERENCE:
                ret &= mc_serialize_string(buffer, input->_.object_name);
                break;
            case REPRESENTATION_AS_XML_STRING:
                ret &= mc_serialize_string(buffer, input->_.xml_string_represenatation);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool deserialize_OBJK_RepresentationRefAndXMLFormats(mcBuffer* buffer, OBJK_RepresentationRefAndXMLFormats* output)
{
    bool ret = true;
    ret &= mc_deserialize_uint8_t(buffer, &output->format);
    if (ret)
    {
        switch(output->format)
        {
            case REPRESENTATION_BY_REFERENCE:
                ret &= mc_deserialize_string(buffer, output->_.object_name, STRING_SIZE_MAX);
                break;
            case REPRESENTATION_AS_XML_STRING:
                ret &= mc_deserialize_string(buffer, output->_.xml_string_represenatation, STRING_SIZE_MAX);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool serialize_OBJK_RepresentationBinAndXMLFormats(mcBuffer* buffer, const OBJK_RepresentationBinAndXMLFormats* input)
{
    bool ret = true;
    ret &= mc_serialize_uint8_t(buffer, input->format);
    if (ret)
    {
        switch(input->format)
        {
            case REPRESENTATION_IN_BINARY:
                ret &= serialize_BinarySequence_t(buffer, &input->_.binary_representation);
                break;
            case REPRESENTATION_AS_XML_STRING:
                ret &= mc_serialize_string(buffer, input->_.string_represenatation);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool deserialize_OBJK_RepresentationBinAndXMLFormats(mcBuffer* buffer, OBJK_RepresentationBinAndXMLFormats* output)
{
    bool ret = true;
    ret &= mc_deserialize_uint8_t(buffer, &output->format);
    if (ret)
    {
        switch(output->format)
        {
            case REPRESENTATION_IN_BINARY:
                ret &= deserialize_BinarySequence_t(buffer, &output->_.binary_representation);
                break;
            case REPRESENTATION_AS_XML_STRING:
                ret &= mc_deserialize_string(buffer, output->_.string_represenatation, STRING_SIZE_MAX);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool serialize_OBJK_RepresentationRefAndXML_Base(mcBuffer* buffer, const OBJK_RepresentationRefAndXML_Base* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationRefAndXMLFormats(buffer, &input->representation);
    return ret;
}

bool deserialize_OBJK_RepresentationRefAndXML_Base(mcBuffer* buffer, OBJK_RepresentationRefAndXML_Base* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationRefAndXMLFormats(buffer, &output->representation);
    return ret;
}

bool serialize_OBJK_RepresentationBinAndXML_Base(mcBuffer* buffer, const OBJK_RepresentationBinAndXML_Base* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationBinAndXMLFormats(buffer, &input->representation);
    return ret;
}

bool deserialize_OBJK_RepresentationBinAndXML_Base(mcBuffer* buffer, OBJK_RepresentationBinAndXML_Base* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationBinAndXMLFormats(buffer, &output->representation);
    return ret;
}

bool serialize_OBJK_Representation3_Base(mcBuffer* buffer, const OBJK_Representation3_Base* input)
{
    bool ret = true;
    ret &= serialize_OBJK_Representation3Formats(buffer, &input->representation);
    return ret;
}

bool deserialize_OBJK_Representation3_Base(mcBuffer* buffer, OBJK_Representation3_Base* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_Representation3Formats(buffer, &output->representation);
    return ret;
}

bool serialize_OBJK_QOSPROFILE_Representation(mcBuffer* buffer, const OBJK_QOSPROFILE_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationRefAndXML_Base(buffer, &input->base);
    return ret;
}

bool deserialize_OBJK_QOSPROFILE_Representation(mcBuffer* buffer, OBJK_QOSPROFILE_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationRefAndXML_Base(buffer, &output->base);
    return ret;
}

bool serialize_OBJK_TYPE_Representation(mcBuffer* buffer, const OBJK_TYPE_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationRefAndXML_Base(buffer, &input->base);
    return ret;
}

bool deserialize_OBJK_TYPE_Representation(mcBuffer* buffer, OBJK_TYPE_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationRefAndXML_Base(buffer, &output->base);
    return ret;
}

bool serialize_OBJK_DOMAIN_Representation(mcBuffer* buffer, const OBJK_DOMAIN_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationRefAndXML_Base(buffer, &input->base);
    return ret;
}

bool deserialize_OBJK_DOMAIN_Representation(mcBuffer* buffer, OBJK_DOMAIN_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationRefAndXML_Base(buffer, &output->base);
    return ret;
}

bool serialize_OBJK_APPLICATION_Representation(mcBuffer* buffer, const OBJK_APPLICATION_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationRefAndXML_Base(buffer, &input->base);
    return ret;
}

bool deserialize_OBJK_APPLICATION_Representation(mcBuffer* buffer, OBJK_APPLICATION_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationRefAndXML_Base(buffer, &output->base);
    return ret;
}

bool serialize_OBJK_PUBLISHER_Representation(mcBuffer* buffer, const OBJK_PUBLISHER_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationBinAndXML_Base(buffer, &input->base);
    ret &= serialize_ObjectId(buffer, &input->participant_id);
    return ret;
}

bool deserialize_OBJK_PUBLISHER_Representation(mcBuffer* buffer, OBJK_PUBLISHER_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationBinAndXML_Base(buffer, &output->base);
    ret &= deserialize_ObjectId(buffer, &output->participant_id);
    return ret;
}

bool serialize_OBJK_SUBSCRIBER_Representation(mcBuffer* buffer, const OBJK_SUBSCRIBER_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_RepresentationBinAndXML_Base(buffer, &input->base);
    ret &= serialize_ObjectId(buffer, &input->participant_id);
    return ret;
}

bool deserialize_OBJK_SUBSCRIBER_Representation(mcBuffer* buffer, OBJK_SUBSCRIBER_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_RepresentationBinAndXML_Base(buffer, &output->base);
    ret &= deserialize_ObjectId(buffer, &output->participant_id);
    return ret;
}

bool serialize_DATAWRITER_Representation(mcBuffer* buffer, const DATAWRITER_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_Representation3_Base(buffer, &input->base);
    ret &= serialize_ObjectId(buffer, &input->publisher_id);
    return ret;
}

bool deserialize_DATAWRITER_Representation(mcBuffer* buffer, DATAWRITER_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_Representation3_Base(buffer, &output->base);
    ret &= deserialize_ObjectId(buffer, &output->publisher_id);
    return ret;
}

bool serialize_DATAREADER_Representation(mcBuffer* buffer, const DATAREADER_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_Representation3_Base(buffer, &input->base);
    ret &= serialize_ObjectId(buffer, &input->subscriber_id);
    return ret;
}

bool deserialize_DATAREADER_Representation(mcBuffer* buffer, DATAREADER_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_Representation3_Base(buffer, &output->base);
    ret &= deserialize_ObjectId(buffer, &output->subscriber_id);
    return ret;
}

bool serialize_OBJK_PARTICIPANT_Representation(mcBuffer* buffer, const OBJK_PARTICIPANT_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_Representation3_Base(buffer, &input->base);
    ret &= mc_serialize_int16_t(buffer, input->domain_id);
    return ret;
}

bool deserialize_OBJK_PARTICIPANT_Representation(mcBuffer* buffer, OBJK_PARTICIPANT_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_Representation3_Base(buffer, &output->base);
    ret &= mc_deserialize_int16_t(buffer, &output->domain_id);
    return ret;
}

bool serialize_OBJK_TOPIC_Representation(mcBuffer* buffer, const OBJK_TOPIC_Representation* input)
{
    bool ret = true;
    ret &= serialize_OBJK_Representation3_Base(buffer, &input->base);
    ret &= serialize_ObjectId(buffer, &input->participant_id);
    return ret;
}

bool deserialize_OBJK_TOPIC_Representation(mcBuffer* buffer, OBJK_TOPIC_Representation* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_Representation3_Base(buffer, &output->base);
    ret &= deserialize_ObjectId(buffer, &output->participant_id);
    return ret;
}

bool serialize_OBJK_DomainParticipant_Binary(mcBuffer* buffer, const OBJK_DomainParticipant_Binary* input)
{
    bool ret = true;
    ret &= mc_serialize_bool(buffer, input->optional_domain_reference);
    if(input->optional_domain_reference == true)
    {
        ret &= mc_serialize_string(buffer, input->domain_reference);
    }

    ret &= mc_serialize_bool(buffer, input->optional_qos_profile_reference);
    if(input->optional_qos_profile_reference == true)
    {
        ret &= mc_serialize_string(buffer, input->qos_profile_reference);
    }

    return ret;
}

bool deserialize_OBJK_DomainParticipant_Binary(mcBuffer* buffer, OBJK_DomainParticipant_Binary* output)
{
    bool ret = true;
    ret &= mc_deserialize_bool(buffer, &output->optional_domain_reference);
    if(output->optional_domain_reference == true)
    {
            ret &= mc_deserialize_string(buffer, output->domain_reference, STRING_SIZE_MAX);
    }

    ret &= mc_deserialize_bool(buffer, &output->optional_qos_profile_reference);
    if(output->optional_qos_profile_reference == true)
    {
            ret &= mc_deserialize_string(buffer, output->qos_profile_reference, STRING_SIZE_MAX);
    }

    return ret;
}

bool serialize_OBJK_Topic_Binary(mcBuffer* buffer, const OBJK_Topic_Binary* input)
{
    bool ret = true;
    ret &= mc_serialize_string(buffer, input->topic_name);
    ret &= mc_serialize_bool(buffer, input->optional_type_reference);
    if(input->optional_type_reference == true)
    {
        ret &= mc_serialize_string(buffer, input->type_reference);
    }

    ret &= mc_serialize_bool(buffer, input->optional_type_name);
    if(input->optional_type_name == true)
    {
        ret &= mc_serialize_string(buffer, input->type_name);
    }

    return ret;
}

bool deserialize_OBJK_Topic_Binary(mcBuffer* buffer, OBJK_Topic_Binary* output)
{
    bool ret = true;
    ret &= mc_deserialize_string(buffer, output->topic_name, STRING_SIZE_MAX);
    ret &= mc_deserialize_bool(buffer, &output->optional_type_reference);
    if(output->optional_type_reference == true)
    {
            ret &= mc_deserialize_string(buffer, output->type_reference, STRING_SIZE_MAX);
    }

    ret &= mc_deserialize_bool(buffer, &output->optional_type_name);
    if(output->optional_type_name == true)
    {
            ret &= mc_deserialize_string(buffer, output->type_name, STRING_SIZE_MAX);
    }

    return ret;
}

bool serialize_OBJK_Publisher_Binary_Qos(mcBuffer* buffer, const OBJK_Publisher_Binary_Qos* input)
{
    bool ret = true;
    ret &= mc_serialize_bool(buffer, input->optional_partitions);
    if(input->optional_partitions == true)
    {
        ret &= serialize_StringSequence_t(buffer, &input->partitions);
    }

    ret &= mc_serialize_bool(buffer, input->optional_group_data);
    if(input->optional_group_data == true)
    {
        ret &= serialize_BinarySequence_t(buffer, &input->group_data);
    }

    return ret;
}

bool deserialize_OBJK_Publisher_Binary_Qos(mcBuffer* buffer, OBJK_Publisher_Binary_Qos* output)
{
    bool ret = true;
    ret &= mc_deserialize_bool(buffer, &output->optional_partitions);
    if(output->optional_partitions == true)
    {
            ret &= deserialize_StringSequence_t(buffer, &output->partitions);
    }

    ret &= mc_deserialize_bool(buffer, &output->optional_group_data);
    if(output->optional_group_data == true)
    {
            ret &= deserialize_BinarySequence_t(buffer, &output->group_data);
    }

    return ret;
}

bool serialize_OBJK_Publisher_Binary(mcBuffer* buffer, const OBJK_Publisher_Binary* input)
{
    bool ret = true;
    ret &= mc_serialize_bool(buffer, input->optional_publisher_name);
    if(input->optional_publisher_name == true)
    {
        ret &= mc_serialize_string(buffer, input->publisher_name);
    }

    ret &= mc_serialize_bool(buffer, input->optional_qos);
    if(input->optional_qos == true)
    {
        ret &= serialize_OBJK_Publisher_Binary_Qos(buffer, &input->qos);
    }

    return ret;
}

bool deserialize_OBJK_Publisher_Binary(mcBuffer* buffer, OBJK_Publisher_Binary* output)
{
    bool ret = true;
    ret &= mc_deserialize_bool(buffer, &output->optional_publisher_name);
    if(output->optional_publisher_name == true)
    {
            ret &= mc_deserialize_string(buffer, output->publisher_name, STRING_SIZE_MAX);
    }

    ret &= mc_deserialize_bool(buffer, &output->optional_qos);
    if(output->optional_qos == true)
    {
            ret &= deserialize_OBJK_Publisher_Binary_Qos(buffer, &output->qos);
    }

    return ret;
}

bool serialize_OBJK_Subscriber_Binary_Qos(mcBuffer* buffer, const OBJK_Subscriber_Binary_Qos* input)
{
    bool ret = true;
    ret &= mc_serialize_bool(buffer, input->optional_partitions);
    if(input->optional_partitions == true)
    {
        ret &= serialize_StringSequence_t(buffer, &input->partitions);
    }

    ret &= mc_serialize_bool(buffer, input->optional_group_data);
    if(input->optional_group_data == true)
    {
        ret &= serialize_BinarySequence_t(buffer, &input->group_data);
    }

    return ret;
}

bool deserialize_OBJK_Subscriber_Binary_Qos(mcBuffer* buffer, OBJK_Subscriber_Binary_Qos* output)
{
    bool ret = true;
    ret &= mc_deserialize_bool(buffer, &output->optional_partitions);
    if(output->optional_partitions == true)
    {
            ret &= deserialize_StringSequence_t(buffer, &output->partitions);
    }

    ret &= mc_deserialize_bool(buffer, &output->optional_group_data);
    if(output->optional_group_data == true)
    {
            ret &= deserialize_BinarySequence_t(buffer, &output->group_data);
    }

    return ret;
}

bool serialize_OBJK_Subscriber_Binary(mcBuffer* buffer, const OBJK_Subscriber_Binary* input)
{
    bool ret = true;
    ret &= mc_serialize_bool(buffer, input->optional_subscriber_name);
    if(input->optional_subscriber_name == true)
    {
        ret &= mc_serialize_string(buffer, input->subscriber_name);
    }

    ret &= mc_serialize_bool(buffer, input->optional_qos);
    if(input->optional_qos == true)
    {
        ret &= serialize_OBJK_Subscriber_Binary_Qos(buffer, &input->qos);
    }

    return ret;
}

bool deserialize_OBJK_Subscriber_Binary(mcBuffer* buffer, OBJK_Subscriber_Binary* output)
{
    bool ret = true;
    ret &= mc_deserialize_bool(buffer, &output->optional_subscriber_name);
    if(output->optional_subscriber_name == true)
    {
            ret &= mc_deserialize_string(buffer, output->subscriber_name, STRING_SIZE_MAX);
    }

    ret &= mc_deserialize_bool(buffer, &output->optional_qos);
    if(output->optional_qos == true)
    {
            ret &= deserialize_OBJK_Subscriber_Binary_Qos(buffer, &output->qos);
    }

    return ret;
}

bool serialize_OBJK_Endpoint_QosBinary(mcBuffer* buffer, const OBJK_Endpoint_QosBinary* input)
{
    bool ret = true;
    ret &= mc_serialize_uint16_t(buffer, input->qos_flags);
    ret &= mc_serialize_bool(buffer, input->optional_history_depth);
    if(input->optional_history_depth == true)
    {
        ret &= mc_serialize_uint16_t(buffer, input->history_depth);
    }

    ret &= mc_serialize_bool(buffer, input->optional_deadline_msec);
    if(input->optional_deadline_msec == true)
    {
        ret &= mc_serialize_uint32_t(buffer, input->deadline_msec);
    }

    ret &= mc_serialize_bool(buffer, input->optional_lifespan_msec);
    if(input->optional_lifespan_msec == true)
    {
        ret &= mc_serialize_uint32_t(buffer, input->lifespan_msec);
    }

    ret &= mc_serialize_bool(buffer, input->optional_user_data);
    if(input->optional_user_data == true)
    {
        ret &= serialize_BinarySequence_t(buffer, &input->user_data);
    }

    return ret;
}

bool deserialize_OBJK_Endpoint_QosBinary(mcBuffer* buffer, OBJK_Endpoint_QosBinary* output)
{
    bool ret = true;
    ret &= mc_deserialize_uint16_t(buffer, &output->qos_flags);
    ret &= mc_deserialize_bool(buffer, &output->optional_history_depth);
    if(output->optional_history_depth == true)
    {
            ret &= mc_deserialize_uint16_t(buffer, &output->history_depth);
    }

    ret &= mc_deserialize_bool(buffer, &output->optional_deadline_msec);
    if(output->optional_deadline_msec == true)
    {
            ret &= mc_deserialize_uint32_t(buffer, &output->deadline_msec);
    }

    ret &= mc_deserialize_bool(buffer, &output->optional_lifespan_msec);
    if(output->optional_lifespan_msec == true)
    {
            ret &= mc_deserialize_uint32_t(buffer, &output->lifespan_msec);
    }

    ret &= mc_deserialize_bool(buffer, &output->optional_user_data);
    if(output->optional_user_data == true)
    {
            ret &= deserialize_BinarySequence_t(buffer, &output->user_data);
    }

    return ret;
}

bool serialize_OBJK_DataWriter_Binary_Qos(mcBuffer* buffer, const OBJK_DataWriter_Binary_Qos* input)
{
    bool ret = true;
    ret &= serialize_OBJK_Endpoint_QosBinary(buffer, &input->base);
    ret &= mc_serialize_bool(buffer, input->optional_ownership_strength);
    if(input->optional_ownership_strength == true)
    {
        ret &= mc_serialize_uint64_t(buffer, input->ownership_strength);
    }

    return ret;
}

bool deserialize_OBJK_DataWriter_Binary_Qos(mcBuffer* buffer, OBJK_DataWriter_Binary_Qos* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_Endpoint_QosBinary(buffer, &output->base);
    ret &= mc_deserialize_bool(buffer, &output->optional_ownership_strength);
    if(output->optional_ownership_strength == true)
    {
            ret &= mc_deserialize_uint64_t(buffer, &output->ownership_strength);
    }

    return ret;
}

bool serialize_OBJK_DataReader_Binary_Qos(mcBuffer* buffer, const OBJK_DataReader_Binary_Qos* input)
{
    bool ret = true;
    ret &= serialize_OBJK_Endpoint_QosBinary(buffer, &input->base);
    ret &= mc_serialize_bool(buffer, input->optional_timebasedfilter_msec);
    if(input->optional_timebasedfilter_msec == true)
    {
        ret &= mc_serialize_uint64_t(buffer, input->timebasedfilter_msec);
    }

    ret &= mc_serialize_bool(buffer, input->optional_contentbased_filter);
    if(input->optional_contentbased_filter == true)
    {
        ret &= mc_serialize_string(buffer, input->contentbased_filter);
    }

    return ret;
}

bool deserialize_OBJK_DataReader_Binary_Qos(mcBuffer* buffer, OBJK_DataReader_Binary_Qos* output)
{
    bool ret = true;
    ret &= deserialize_OBJK_Endpoint_QosBinary(buffer, &output->base);
    ret &= mc_deserialize_bool(buffer, &output->optional_timebasedfilter_msec);
    if(output->optional_timebasedfilter_msec == true)
    {
            ret &= mc_deserialize_uint64_t(buffer, &output->timebasedfilter_msec);
    }

    ret &= mc_deserialize_bool(buffer, &output->optional_contentbased_filter);
    if(output->optional_contentbased_filter == true)
    {
            ret &= mc_deserialize_string(buffer, output->contentbased_filter, STRING_SIZE_MAX);
    }

    return ret;
}

bool serialize_OBJK_DataReader_Binary(mcBuffer* buffer, const OBJK_DataReader_Binary* input)
{
    bool ret = true;
    ret &= mc_serialize_string(buffer, input->topic_name);
    ret &= mc_serialize_bool(buffer, input->optional_qos);
    if(input->optional_qos == true)
    {
        ret &= serialize_OBJK_DataReader_Binary_Qos(buffer, &input->qos);
    }

    return ret;
}

bool deserialize_OBJK_DataReader_Binary(mcBuffer* buffer, OBJK_DataReader_Binary* output)
{
    bool ret = true;
    ret &= mc_deserialize_string(buffer, output->topic_name, STRING_SIZE_MAX);
    ret &= mc_deserialize_bool(buffer, &output->optional_qos);
    if(output->optional_qos == true)
    {
            ret &= deserialize_OBJK_DataReader_Binary_Qos(buffer, &output->qos);
    }

    return ret;
}

bool serialize_OBJK_DataWriter_Binary(mcBuffer* buffer, const OBJK_DataWriter_Binary* input)
{
    bool ret = true;
    ret &= mc_serialize_string(buffer, input->topic_name);
    ret &= mc_serialize_bool(buffer, input->optional_qos);
    if(input->optional_qos == true)
    {
        ret &= serialize_OBJK_DataWriter_Binary_Qos(buffer, &input->qos);
    }

    return ret;
}

bool deserialize_OBJK_DataWriter_Binary(mcBuffer* buffer, OBJK_DataWriter_Binary* output)
{
    bool ret = true;
    ret &= mc_deserialize_string(buffer, output->topic_name, STRING_SIZE_MAX);
    ret &= mc_deserialize_bool(buffer, &output->optional_qos);
    if(output->optional_qos == true)
    {
            ret &= deserialize_OBJK_DataWriter_Binary_Qos(buffer, &output->qos);
    }

    return ret;
}

bool serialize_ObjectVariant(mcBuffer* buffer, const ObjectVariant* input)
{
    bool ret = true;
    ret &= mc_serialize_uint8_t(buffer, input->kind);
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

bool deserialize_ObjectVariant(mcBuffer* buffer, ObjectVariant* output)
{
    bool ret = true;
    ret &= mc_deserialize_uint8_t(buffer, &output->kind);
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

bool serialize_CreationMode(mcBuffer* buffer, const CreationMode* input)
{
    bool ret = true;
    ret &= mc_serialize_bool(buffer, input->reuse);
    ret &= mc_serialize_bool(buffer, input->replace);
    return ret;
}

bool deserialize_CreationMode(mcBuffer* buffer, CreationMode* output)
{
    bool ret = true;
    ret &= mc_deserialize_bool(buffer, &output->reuse);
    ret &= mc_deserialize_bool(buffer, &output->replace);
    return ret;
}

bool serialize_RequestId(mcBuffer* buffer, const RequestId* input)
{
    bool ret = true;
    ret &= mc_serialize_array_uint8_t(buffer, input->data, 2);
    return ret;
}

bool deserialize_RequestId(mcBuffer* buffer, RequestId* output)
{
    bool ret = true;
    ret &= mc_deserialize_array_uint8_t(buffer, output->data, 2);
    return ret;
}

bool serialize_ResultStatus(mcBuffer* buffer, const ResultStatus* input)
{
    bool ret = true;
    ret &= mc_serialize_uint8_t(buffer, input->status);
    ret &= mc_serialize_uint8_t(buffer, input->implementation_status);
    return ret;
}

bool deserialize_ResultStatus(mcBuffer* buffer, ResultStatus* output)
{
    bool ret = true;
    ret &= mc_deserialize_uint8_t(buffer, &output->status);
    ret &= mc_deserialize_uint8_t(buffer, &output->implementation_status);
    return ret;
}

bool serialize_BaseObjectRequest(mcBuffer* buffer, const BaseObjectRequest* input)
{
    bool ret = true;
    ret &= serialize_RequestId(buffer, &input->request_id);
    ret &= serialize_ObjectId(buffer, &input->object_id);
    return ret;
}

bool deserialize_BaseObjectRequest(mcBuffer* buffer, BaseObjectRequest* output)
{
    bool ret = true;
    ret &= deserialize_RequestId(buffer, &output->request_id);
    ret &= deserialize_ObjectId(buffer, &output->object_id);
    return ret;
}

bool serialize_AGENT_ActivityInfo(mcBuffer* buffer, const AGENT_ActivityInfo* input)
{
    bool ret = true;
    ret &= mc_serialize_int16_t(buffer, input->availibility);
    ret &= serialize_TransportLocatorSeq(buffer, &input->address_seq);
    return ret;
}

bool deserialize_AGENT_ActivityInfo(mcBuffer* buffer, AGENT_ActivityInfo* output)
{
    bool ret = true;
    ret &= mc_deserialize_int16_t(buffer, &output->availibility);
    ret &= deserialize_TransportLocatorSeq(buffer, &output->address_seq);
    return ret;
}

bool serialize_DATAREADER_ActivityInfo(mcBuffer* buffer, const DATAREADER_ActivityInfo* input)
{
    bool ret = true;
    ret &= mc_serialize_int16_t(buffer, input->highest_acked_num);
    return ret;
}

bool deserialize_DATAREADER_ActivityInfo(mcBuffer* buffer, DATAREADER_ActivityInfo* output)
{
    bool ret = true;
    ret &= mc_deserialize_int16_t(buffer, &output->highest_acked_num);
    return ret;
}

bool serialize_DATAWRITER_ActivityInfo(mcBuffer* buffer, const DATAWRITER_ActivityInfo* input)
{
    bool ret = true;
    ret &= mc_serialize_int16_t(buffer, input->stream_seq_num);
    ret &= mc_serialize_uint64_t(buffer, input->sample_seq_num);
    return ret;
}

bool deserialize_DATAWRITER_ActivityInfo(mcBuffer* buffer, DATAWRITER_ActivityInfo* output)
{
    bool ret = true;
    ret &= mc_deserialize_int16_t(buffer, &output->stream_seq_num);
    ret &= mc_deserialize_uint64_t(buffer, &output->sample_seq_num);
    return ret;
}

bool serialize_ActivityInfoVariant(mcBuffer* buffer, const ActivityInfoVariant* input)
{
    bool ret = true;
    ret &= mc_serialize_uint8_t(buffer, input->kind);
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

bool deserialize_ActivityInfoVariant(mcBuffer* buffer, ActivityInfoVariant* output)
{
    bool ret = true;
    ret &= mc_deserialize_uint8_t(buffer, &output->kind);
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

bool serialize_ObjectInfo(mcBuffer* buffer, const ObjectInfo* input)
{
    bool ret = true;
    ret &= mc_serialize_bool(buffer, input->optional_config);
    if(input->optional_config == true)
    {
        ret &= serialize_ObjectVariant(buffer, &input->config);
    }

    ret &= mc_serialize_bool(buffer, input->optional_activity);
    if(input->optional_activity == true)
    {
        ret &= serialize_ActivityInfoVariant(buffer, &input->activity);
    }

    return ret;
}

bool deserialize_ObjectInfo(mcBuffer* buffer, ObjectInfo* output)
{
    bool ret = true;
    ret &= mc_deserialize_bool(buffer, &output->optional_config);
    if(output->optional_config == true)
    {
            ret &= deserialize_ObjectVariant(buffer, &output->config);
    }

    ret &= mc_deserialize_bool(buffer, &output->optional_activity);
    if(output->optional_activity == true)
    {
            ret &= deserialize_ActivityInfoVariant(buffer, &output->activity);
    }

    return ret;
}

bool serialize_BaseObjectReply(mcBuffer* buffer, const BaseObjectReply* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->related_request);
    ret &= serialize_ResultStatus(buffer, &input->result);
    return ret;
}

bool deserialize_BaseObjectReply(mcBuffer* buffer, BaseObjectReply* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->related_request);
    ret &= deserialize_ResultStatus(buffer, &output->result);
    return ret;
}

bool serialize_DataDeliveryControl(mcBuffer* buffer, const DataDeliveryControl* input)
{
    bool ret = true;
    ret &= mc_serialize_uint16_t(buffer, input->max_samples);
    ret &= mc_serialize_uint16_t(buffer, input->max_elapsed_time);
    ret &= mc_serialize_uint16_t(buffer, input->max_bytes_per_seconds);
    ret &= mc_serialize_uint16_t(buffer, input->min_pace_period);
    return ret;
}

bool deserialize_DataDeliveryControl(mcBuffer* buffer, DataDeliveryControl* output)
{
    bool ret = true;
    ret &= mc_deserialize_uint16_t(buffer, &output->max_samples);
    ret &= mc_deserialize_uint16_t(buffer, &output->max_elapsed_time);
    ret &= mc_deserialize_uint16_t(buffer, &output->max_bytes_per_seconds);
    ret &= mc_deserialize_uint16_t(buffer, &output->min_pace_period);
    return ret;
}

bool serialize_ReadSpecification(mcBuffer* buffer, const ReadSpecification* input)
{
    bool ret = true;
    ret &= mc_serialize_uint8_t(buffer, input->input_stream_id);
    ret &= mc_serialize_uint8_t(buffer, input->data_format);
    ret &= mc_serialize_bool(buffer, input->optional_content_filter_expression);
    if(input->optional_content_filter_expression == true)
    {
        ret &= mc_serialize_string(buffer, input->content_filter_expression);
    }

    ret &= mc_serialize_bool(buffer, input->optional_delivery_control);
    if(input->optional_delivery_control == true)
    {
        ret &= serialize_DataDeliveryControl(buffer, &input->delivery_control);
    }

    return ret;
}

bool deserialize_ReadSpecification(mcBuffer* buffer, ReadSpecification* output)
{
    bool ret = true;
    ret &= mc_deserialize_uint8_t(buffer, &output->input_stream_id);
    ret &= mc_deserialize_uint8_t(buffer, &output->data_format);
    ret &= mc_deserialize_bool(buffer, &output->optional_content_filter_expression);
    if(output->optional_content_filter_expression == true)
    {
        ret &= mc_deserialize_string(buffer, output->content_filter_expression, STRING_SIZE_MAX);
    }

    ret &= mc_deserialize_bool(buffer, &output->optional_delivery_control);
    if(output->optional_delivery_control == true)
    {
        ret &= deserialize_DataDeliveryControl(buffer, &output->delivery_control);
    }

    return ret;
}

bool serialize_SeqNumberAndTimestamp(mcBuffer* buffer, const SeqNumberAndTimestamp* input)
{
    bool ret = true;
    ret &= mc_serialize_uint32_t(buffer, input->sequence_number);
    ret &= mc_serialize_uint32_t(buffer, input->session_time_offset);
    return ret;
}

bool deserialize_SeqNumberAndTimestamp(mcBuffer* buffer, SeqNumberAndTimestamp* output)
{
    bool ret = true;
    ret &= mc_deserialize_uint32_t(buffer, &output->sequence_number);
    ret &= mc_deserialize_uint32_t(buffer, &output->session_time_offset);
    return ret;
}

bool serialize_SampleInfoDetail(mcBuffer* buffer, const SampleInfoDetail* input)
{
    bool ret = true;
    ret &= mc_serialize_uint32_t(buffer, input->format);
    if (ret)
    {
        switch(input->format)
        {
            case FORMAT_EMPTY:
                ret &= mc_serialize_uint32_t(buffer, input->_.sequence_number);
                break;
            case FORMAT_SEQNUM:
                ret &= mc_serialize_uint32_t(buffer, input->_.session_time_offset);
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

bool deserialize_SampleInfoDetail(mcBuffer* buffer, SampleInfoDetail* output)
{
    bool ret = true;
    ret &= mc_deserialize_uint32_t(buffer, &output->format);
    if (ret)
    {
        switch(output->format)
        {
            case FORMAT_EMPTY:
                ret &= mc_deserialize_uint32_t(buffer, &output->_.sequence_number);
                break;
            case FORMAT_SEQNUM:
                ret &= mc_deserialize_uint32_t(buffer, &output->_.session_time_offset);
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

bool serialize_SampleInfo(mcBuffer* buffer, const SampleInfo* input)
{
    bool ret = true;
    ret &= mc_serialize_uint8_t(buffer, input->state);
    ret &= serialize_SampleInfoDetail(buffer, &input->detail);
    return ret;
}

bool deserialize_SampleInfo(mcBuffer* buffer, SampleInfo* output)
{
    bool ret = true;
    ret &= mc_deserialize_uint8_t(buffer, &output->state);
    ret &= deserialize_SampleInfoDetail(buffer, &output->detail);
    return ret;
}

bool serialize_SampleInfoDelta(mcBuffer* buffer, const SampleInfoDelta* input)
{
    bool ret = true;
    ret &= mc_serialize_uint8_t(buffer, input->state);
    ret &= mc_serialize_uint8_t(buffer, input->seq_number_delta);
    ret &= mc_serialize_uint16_t(buffer, input->timestamp_delta);
    return ret;
}

bool deserialize_SampleInfoDelta(mcBuffer* buffer, SampleInfoDelta* output)
{
    bool ret = true;
    ret &= mc_deserialize_uint8_t(buffer, &output->state);
    ret &= mc_deserialize_uint8_t(buffer, &output->seq_number_delta);
    ret &= mc_deserialize_uint16_t(buffer, &output->timestamp_delta);
    return ret;
}

bool serialize_SampleData(mcBuffer* buffer, const SampleData* input)
{
    return mc_serialize_sequence_uint8_t(buffer, input->data, input->size);
}

bool deserialize_SampleData(mcBuffer* buffer, SampleData* output)
{
    return mc_deserialize_sequence_uint8_t(buffer, output->data, SAMPLE_DATA_SIZE_MAX, &output->size);
}

bool serialize_SampleDataSeq(mcBuffer* buffer, const SampleDataSeq* input)
{
    bool ret = mc_serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size && ret; i++)
    {
        ret = serialize_SampleData(buffer, &input->data[i]);
    }
    return ret;
}

bool deserialize_SampleDataSeq(mcBuffer* buffer, SampleDataSeq* output)
{
    bool ret = mc_deserialize_uint32_t(buffer, &output->size);
    if(output->size > SAMPLE_DATA_SEQUENCE_MAX)
    {
        buffer->error = true;
        ret = false;
    }
    else
    {
        for(uint32_t i = 0; i < output->size && ret; i++)
        {
            ret = deserialize_SampleData(buffer, &output->data[i]);
        }
    }
    return ret;
}

bool serialize_Sample(mcBuffer* buffer, const Sample* input)
{
    bool ret = true;
    ret &= serialize_SampleInfo(buffer, &input->info);
    ret &= serialize_SampleData(buffer, &input->data);
    return ret;
}

bool deserialize_Sample(mcBuffer* buffer, Sample* output)
{
    bool ret = true;
    ret &= deserialize_SampleInfo(buffer, &output->info);
    ret &= deserialize_SampleData(buffer, &output->data);
    return ret;
}

bool serialize_SampleSeq(mcBuffer* buffer, const SampleSeq* input)
{
    bool ret = mc_serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size && ret; i++)
    {
        ret = serialize_Sample(buffer, &input->data[i]);
    }
    return ret;
}

bool deserialize_SampleSeq(mcBuffer* buffer, SampleSeq* output)
{
    bool ret = mc_deserialize_uint32_t(buffer, &output->size);
    if(output->size > SAMPLE_SEQUENCE_MAX)
    {
        buffer->error = true;
        ret = false;
    }
    else
    {
        for(uint32_t i = 0; i < output->size && ret; i++)
        {
            ret = deserialize_Sample(buffer, &output->data[i]);
        }
    }
    return ret;
}

bool serialize_SampleDelta(mcBuffer* buffer, const SampleDelta* input)
{
    bool ret = true;
    ret &= serialize_SampleInfoDelta(buffer, &input->info_delta);
    ret &= serialize_SampleData(buffer, &input->data);
    return ret;
}

bool deserialize_SampleDelta(mcBuffer* buffer, SampleDelta* output)
{
    bool ret = true;
    ret &= deserialize_SampleInfoDelta(buffer, &output->info_delta);
    ret &= deserialize_SampleData(buffer, &output->data);
    return ret;
}

bool serialize_SampleDeltaSequence(mcBuffer* buffer, const SampleDeltaSequence* input)
{
    bool ret = mc_serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size && ret; i++)
    {
        ret = serialize_SampleDelta(buffer, &input->data[i]);
    }
    return ret;
}

bool deserialize_SampleDeltaSequence(mcBuffer* buffer, SampleDeltaSequence* output)
{
    bool ret = mc_deserialize_uint32_t(buffer, &output->size);
    if(output->size > SAMPLE_DELTA_SEQUENCE_MAX)
    {
        buffer->error = true;
        ret = false;
    }
    else
    {
        for(uint32_t i = 0; i < output->size && ret; i++)
        {
            ret = deserialize_SampleDelta(buffer, &output->data[i]);
        }
    }
    return ret;
}

bool serialize_PackedSamples(mcBuffer* buffer, const PackedSamples* input)
{
    bool ret = true;
    ret &= serialize_SampleInfo(buffer, &input->info_base);
    ret &= serialize_SampleDeltaSequence(buffer, &input->sample_delta_seq);
    return ret;
}

bool deserialize_PackedSamples(mcBuffer* buffer, PackedSamples* output)
{
    bool ret = true;
    ret &= deserialize_SampleInfo(buffer, &output->info_base);
    ret &= deserialize_SampleDeltaSequence(buffer, &output->sample_delta_seq);
    return ret;
}

bool serialize_SamplePackedSeq(mcBuffer* buffer, const SamplePackedSeq* input)
{
    bool ret = mc_serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size && ret; i++)
    {
        ret = serialize_PackedSamples(buffer, &input->data[i]);
    }
    return ret;
}

bool deserialize_SamplePackedSeq(mcBuffer* buffer, SamplePackedSeq* output)
{
    bool ret = mc_deserialize_uint32_t(buffer, &output->size);
    if(output->size > PACKED_SAMPLES_SEQUENCE_MAX)
    {
        buffer->error = true;
        ret = false;
    }
    else
    {
        for(uint32_t i = 0; i < output->size && ret; i++)
        {
            ret = deserialize_PackedSamples(buffer, &output->data[i]);
        }
    }
    return ret;
}

bool serialize_DataRepresentation(mcBuffer* buffer, const DataRepresentation* input)
{
    bool ret = true;
    ret &= mc_serialize_uint8_t(buffer, input->format);
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

bool deserialize_DataRepresentation(mcBuffer* buffer, DataRepresentation* output)
{
    bool ret = true;
    ret &= mc_deserialize_uint8_t(buffer, &output->format);
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

bool serialize_CREATE_CLIENT_Payload(mcBuffer* buffer, const CREATE_CLIENT_Payload* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_CLIENT_Representation(buffer, &input->client_representation);
    return ret;
}

bool deserialize_CREATE_CLIENT_Payload(mcBuffer* buffer, CREATE_CLIENT_Payload* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_CLIENT_Representation(buffer, &output->client_representation);
    return ret;
}

bool serialize_CREATE_Payload(mcBuffer* buffer, const CREATE_Payload* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_ObjectVariant(buffer, &input->object_representation);
    return ret;
}

bool deserialize_CREATE_Payload(mcBuffer* buffer, CREATE_Payload* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_ObjectVariant(buffer, &output->object_representation);
    return ret;
}

bool serialize_GET_INFO_Payload(mcBuffer* buffer, const GET_INFO_Payload* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= mc_serialize_uint32_t(buffer, input->info_mask);
    return ret;
}

bool deserialize_GET_INFO_Payload(mcBuffer* buffer, GET_INFO_Payload* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= mc_deserialize_uint32_t(buffer, &output->info_mask);
    return ret;
}

bool serialize_DELETE_Payload(mcBuffer* buffer, const DELETE_Payload* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    return ret;
}

bool deserialize_DELETE_Payload(mcBuffer* buffer, DELETE_Payload* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    return ret;
}

bool serialize_STATUS_AGENT_Payload(mcBuffer* buffer, const STATUS_AGENT_Payload* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectReply(buffer, &input->base);
    ret &= serialize_AGENT_Representation(buffer, &input->agent_info);
    return ret;
}

bool deserialize_STATUS_AGENT_Payload(mcBuffer* buffer, STATUS_AGENT_Payload* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectReply(buffer, &output->base);
    ret &= deserialize_AGENT_Representation(buffer, &output->agent_info);
    return ret;
}

bool serialize_STATUS_Payload(mcBuffer* buffer, const STATUS_Payload* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectReply(buffer, &input->base);
    return ret;
}

bool deserialize_STATUS_Payload(mcBuffer* buffer, STATUS_Payload* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectReply(buffer, &output->base);
    return ret;
}

bool serialize_INFO_Payload(mcBuffer* buffer, const INFO_Payload* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectReply(buffer, &input->base);
    ret &= serialize_ObjectInfo(buffer, &input->object_info);
    return ret;
}

bool deserialize_INFO_Payload(mcBuffer* buffer, INFO_Payload* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectReply(buffer, &output->base);
    ret &= deserialize_ObjectInfo(buffer, &output->object_info);
    return ret;
}

bool serialize_READ_DATA_Payload(mcBuffer* buffer, const READ_DATA_Payload* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_ReadSpecification(buffer, &input->read_specification);
    return ret;
}

bool deserialize_READ_DATA_Payload(mcBuffer* buffer, READ_DATA_Payload* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_ReadSpecification(buffer, &output->read_specification);
    return ret;
}

bool serialize_WRITE_DATA_Payload_Data(mcBuffer* buffer, const WRITE_DATA_Payload_Data* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    return ret;
}

bool deserialize_WRITE_DATA_Payload_Data(mcBuffer* buffer, WRITE_DATA_Payload_Data* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    return ret;
}

bool serialize_WRITE_DATA_Payload_Sample(mcBuffer* buffer, const WRITE_DATA_Payload_Sample* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_Sample(buffer, &input->sample);
    return ret;
}

bool deserialize_WRITE_DATA_Payload_Sample(mcBuffer* buffer, WRITE_DATA_Payload_Sample* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_Sample(buffer, &output->sample);
    return ret;
}

bool serialize_WRITE_DATA_Payload_DataSeq(mcBuffer* buffer, const WRITE_DATA_Payload_DataSeq* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_SampleDataSeq(buffer, &input->data_seq);
    return ret;
}

bool deserialize_WRITE_DATA_Payload_DataSeq(mcBuffer* buffer, WRITE_DATA_Payload_DataSeq* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_SampleDataSeq(buffer, &output->data_seq);
    return ret;
}

bool serialize_WRITE_DATA_Payload_SampleSeq(mcBuffer* buffer, const WRITE_DATA_Payload_SampleSeq* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_SampleSeq(buffer, &input->sample_seq);
    return ret;
}

bool deserialize_WRITE_DATA_Payload_SampleSeq(mcBuffer* buffer, WRITE_DATA_Payload_SampleSeq* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_SampleSeq(buffer, &output->sample_seq);
    return ret;
}

bool serialize_WRITE_DATA_Payload_PackedSamples(mcBuffer* buffer, const WRITE_DATA_Payload_PackedSamples* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_PackedSamples(buffer, &input->packed_samples);
    return ret;
}

bool deserialize_WRITE_DATA_Payload_PackedSamples(mcBuffer* buffer, WRITE_DATA_Payload_PackedSamples* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_PackedSamples(buffer, &output->packed_samples);
    return ret;
}

bool serialize_DATA_Payload_Data(mcBuffer* buffer, const DATA_Payload_Data* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_SampleData(buffer, &input->data);
    return ret;
}

bool deserialize_DATA_Payload_Data(mcBuffer* buffer, DATA_Payload_Data* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_SampleData(buffer, &output->data);
    return ret;
}

bool serialize_DATA_Payload_Sample(mcBuffer* buffer, const DATA_Payload_Sample* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_Sample(buffer, &input->sample);
    return ret;
}

bool deserialize_DATA_Payload_Sample(mcBuffer* buffer, DATA_Payload_Sample* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_Sample(buffer, &output->sample);
    return ret;
}

bool serialize_DATA_Payload_DataSeq(mcBuffer* buffer, const DATA_Payload_DataSeq* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_SampleDataSeq(buffer, &input->data_seq);
    return ret;
}

bool deserialize_DATA_Payload_DataSeq(mcBuffer* buffer, DATA_Payload_DataSeq* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_SampleDataSeq(buffer, &output->data_seq);
    return ret;
}

bool serialize_DATA_Payload_SampleSeq(mcBuffer* buffer, const DATA_Payload_SampleSeq* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_SampleSeq(buffer, &input->sample_seq);
    return ret;
}

bool deserialize_DATA_Payload_SampleSeq(mcBuffer* buffer, DATA_Payload_SampleSeq* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_SampleSeq(buffer, &output->sample_seq);
    return ret;
}

bool serialize_DATA_Payload_PackedSamples(mcBuffer* buffer, const DATA_Payload_PackedSamples* input)
{
    bool ret = true;
    ret &= serialize_BaseObjectRequest(buffer, &input->base);
    ret &= serialize_PackedSamples(buffer, &input->packed_samples);
    return ret;
}

bool deserialize_DATA_Payload_PackedSamples(mcBuffer* buffer, DATA_Payload_PackedSamples* output)
{
    bool ret = true;
    ret &= deserialize_BaseObjectRequest(buffer, &output->base);
    ret &= deserialize_PackedSamples(buffer, &output->packed_samples);
    return ret;
}

bool serialize_ACKNACK_Payload(mcBuffer* buffer, const ACKNACK_Payload* input)
{
    bool ret = true;
    ret &= mc_serialize_uint16_t(buffer, input->first_unacked_seq_num);
    ret &= mc_serialize_array_uint8_t(buffer, input->nack_bitmap, 2);
    return ret;
}

bool deserialize_ACKNACK_Payload(mcBuffer* buffer, ACKNACK_Payload* output)
{
    bool ret = true;
    ret &= mc_deserialize_uint16_t(buffer, &output->first_unacked_seq_num);
    ret &= mc_deserialize_array_uint8_t(buffer, output->nack_bitmap, 2);
    return ret;
}

bool serialize_HEARTBEAT_Payload(mcBuffer* buffer, const HEARTBEAT_Payload* input)
{
    bool ret = true;
    ret &= mc_serialize_uint16_t(buffer, input->first_unacked_seq_nr);
    ret &= mc_serialize_uint16_t(buffer, input->last_unacked_seq_nr);
    return ret;
}

bool deserialize_HEARTBEAT_Payload(mcBuffer* buffer, HEARTBEAT_Payload* output)
{
    bool ret = true;
    ret &= mc_deserialize_uint16_t(buffer, &output->first_unacked_seq_nr);
    ret &= mc_deserialize_uint16_t(buffer, &output->last_unacked_seq_nr);
    return ret;
}

