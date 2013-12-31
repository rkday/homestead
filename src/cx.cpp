/**
 * @file cx.h class definition wrapping Cx
 *
 * Project Clearwater - IMS in the Cloud
 * Copyright (C) 2013  Metaswitch Networks Ltd
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version, along with the "Special Exception" for use of
 * the program along with SSL, set forth below. This program is distributed
 * in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details. You should have received a copy of the GNU General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * The author can be reached by email at clearwater@metaswitch.com or by
 * post at Metaswitch Networks Ltd, 100 Church St, Enfield EN2 6BQ, UK
 *
 * Special Exception
 * Metaswitch Networks Ltd  grants you permission to copy, modify,
 * propagate, and distribute a work formed by combining OpenSSL with The
 * Software, or a work derivative of such a combination, even if such
 * copying, modification, propagation, or distribution would otherwise
 * violate the terms of the GPL. You must comply with the GPL in all
 * respects for all of the code used other than OpenSSL.
 * "OpenSSL" means OpenSSL toolkit software distributed by the OpenSSL
 * Project and licensed under the OpenSSL Licenses, or a work based on such
 * software and licensed under the OpenSSL Licenses.
 * "OpenSSL Licenses" means the OpenSSL License and Original SSLeay License
 * under which the OpenSSL Project distributes the OpenSSL toolkit software,
 * as those licenses appear in the file LICENSE-OPENSSL.
 */

#include "cx.h"

#include <string>
#include <sstream>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>

using namespace Cx;

Dictionary::Dictionary() :
  TGPP("3GPP"),
  CX("Cx"),
  USER_AUTHORIZATION_REQUEST("3GPP/User-Authorization-Request"),
  USER_AUTHORIZATION_ANSWER("3GPP/User-Authorization-Answer"),
  LOCATION_INFO_REQUEST("3GPP/Location-Info-Request"),
  LOCATION_INFO_ANSWER("3GPP/Location-Info-Answer"),
  MULTIMEDIA_AUTH_REQUEST("3GPP/Multimedia-Auth-Request"),
  MULTIMEDIA_AUTH_ANSWER("3GPP/Multimedia-Auth-Answer"),
  SERVER_ASSIGNMENT_REQUEST("3GPP/Server-Assignment-Request"),
  SERVER_ASSIGNMENT_ANSWER("3GPP/Server-Assignment-Answer"),
  PUBLIC_IDENTITY("3GPP", "Public-Identity"),
  SIP_AUTH_DATA_ITEM("3GPP", "SIP-Auth-Data-Item"),
  SIP_AUTH_SCHEME("3GPP", "SIP-Authentication-Scheme"),
  SIP_AUTHORIZATION("3GPP", "SIP-Authorization"),
  SIP_NUMBER_AUTH_ITEMS("3GPP", "SIP-Number-Auth-Items"),
  SERVER_NAME("3GPP", "Server-Name"),
  SIP_DIGEST_AUTHENTICATE("3GPP", "SIP-Digest-Authenticate"),
  CX_DIGEST_HA1("3GPP", "Digest-HA1"),
  CX_DIGEST_REALM("3GPP", "Digest-Realm"),
  VISITED_NETWORK_IDENTIFIER("3GPP", "Visited-Network-Identifier"),
  SERVER_CAPABILITIES("3GPP", "Server-Capabilities"),
  MANDATORY_CAPABILITY("3GPP", "Mandatory-Capability"),
  OPTIONAL_CAPABILITY("3GPP", "Optional-Capability"),
  SERVER_ASSIGNMENT_TYPE("3GPP", "Server-Assignment-Type"),
  USER_AUTHORIZATION_TYPE("3GPP", "User-Authorization-Type"),
  ORIGINATING_REQUEST("3GPP", "Originating-Request"),
  USER_DATA_ALREADY_AVAILABLE("3GPP", "User-Data-Already-Available"),
  USER_DATA("3GPP", "User-Data"),
  CX_DIGEST_QOP("3GPP", "Digest-QoP"),
  SIP_AUTHENTICATE("3GPP", "SIP-Authenticate"),
  CONFIDENTIALITY_KEY("3GPP", "Confidentiality-Key"),
  INTEGRITY_KEY("3GPP", "Integrity-Key")
{
}

UserAuthorizationRequest::UserAuthorizationRequest(const Dictionary* dict,
                                                   const std::string& dest_host,
                                                   const std::string& dest_realm,
                                                   const std::string& impi,
                                                   const std::string& impu,
                                                   const std::string& visited_network_identifier,
                                                   const int user_authorization_type) :
                                                   Diameter::Message(dict, dict->USER_AUTHORIZATION_REQUEST)
{
  add_new_session_id();
  add_vendor_spec_app_id();  
  add(Diameter::AVP(dict->AUTH_SESSION_STATE).val_i32(1));
  add_origin();
  add(Diameter::AVP(dict->DESTINATION_HOST).val_str(dest_host));
  add(Diameter::AVP(dict->DESTINATION_REALM).val_str(dest_realm));
  add(Diameter::AVP(dict->USER_NAME).val_str(impi));
  add(Diameter::AVP(dict->PUBLIC_IDENTITY).val_str(impu));
  add(Diameter::AVP(dict->VISITED_NETWORK_IDENTIFIER).val_str(visited_network_identifier));
  add(Diameter::AVP(dict->USER_AUTHORIZATION_TYPE).val_i32(user_authorization_type));
}

UserAuthorizationAnswer::UserAuthorizationAnswer(const Dictionary* dict) :
                                                 Diameter::Message(dict, dict->USER_AUTHORIZATION_ANSWER)
{
}

int UserAuthorizationAnswer::result_code() const
{
  return get_result_code();
}

int UserAuthorizationAnswer::experimental_result_code() const
{
  return get_experimental_result_code();
}

std::string UserAuthorizationAnswer::server_name() const
{
  return get_str_from_avp(((Cx::Dictionary*)dict())->SERVER_NAME);
}

ServerCapabilities UserAuthorizationAnswer::server_capabilities() const
{
  ServerCapabilities server_capabilities;
  Diameter::AVP::iterator avps = begin(((Cx::Dictionary*)dict())->SERVER_CAPABILITIES);
  if (avps != end())
  {
    Diameter::AVP::iterator avps2 = avps->begin(((Cx::Dictionary*)dict())->MANDATORY_CAPABILITY);
    while (avps2 != end())
    {
      server_capabilities.mandatory_capabilities.push_back(avps2->val_i32());
      avps2++;
    }
    avps2 = avps->begin(((Cx::Dictionary*)dict())->OPTIONAL_CAPABILITY);
    while (avps2 != end())
    {
      server_capabilities.optional_capabilities.push_back(avps2->val_i32());
      avps2++;
    }
  }
  return server_capabilities;
}

LocationInfoRequest::LocationInfoRequest(const Dictionary* dict,
                                         const std::string& dest_host,
                                         const std::string& dest_realm,
                                         const std::string originating_request,
                                         const std::string& impu,
                                         const int user_authorization_type) :
                                         Diameter::Message(dict, dict->LOCATION_INFO_REQUEST)
{
  add_new_session_id();
  add_vendor_spec_app_id();
  add(Diameter::AVP(dict->AUTH_SESSION_STATE).val_i32(1));
  add_origin();
  add(Diameter::AVP(dict->DESTINATION_HOST).val_str(dest_host));
  add(Diameter::AVP(dict->DESTINATION_REALM).val_str(dest_realm));
  if (originating_request == "true")
  {
    add(Diameter::AVP(dict->ORIGINATING_REQUEST).val_i32(0));
  }
  add(Diameter::AVP(dict->PUBLIC_IDENTITY).val_str(impu));
  if (user_authorization_type)
  {
    add(Diameter::AVP(dict->USER_AUTHORIZATION_TYPE).val_i32(3));
  }
}

LocationInfoAnswer::LocationInfoAnswer(const Dictionary* dict) :
                                       Diameter::Message(dict, dict->LOCATION_INFO_ANSWER)
{
}

int LocationInfoAnswer::result_code() const
{
  return get_result_code();
}

int LocationInfoAnswer::experimental_result_code() const
{
  return get_experimental_result_code();
}

std::string LocationInfoAnswer::server_name() const
{
  return get_str_from_avp(((Cx::Dictionary*)dict())->SERVER_NAME);
}

ServerCapabilities LocationInfoAnswer::server_capabilities() const
{
  ServerCapabilities server_capabilities;
  Diameter::AVP::iterator avps = begin(((Cx::Dictionary*)dict())->SERVER_CAPABILITIES);
  if (avps != end())
  {
    Diameter::AVP::iterator avps2 = avps->begin(((Cx::Dictionary*)dict())->MANDATORY_CAPABILITY);
    while (avps2 != end())
    {
      server_capabilities.mandatory_capabilities.push_back(avps2->val_i32());
      avps2++;
    }
    avps2 = avps->begin(((Cx::Dictionary*)dict())->OPTIONAL_CAPABILITY);
    while (avps2 != end())
    {
      server_capabilities.optional_capabilities.push_back(avps2->val_i32());
      avps2++;
    }
  }
  return server_capabilities;
}

MultimediaAuthRequest::MultimediaAuthRequest(const Dictionary* dict,
                                             const std::string& dest_realm,
                                             const std::string& dest_host,
                                             const std::string& impi,
                                             const std::string& impu,
                                             const std::string& server_name,
                                             const std::string& sip_auth_scheme,
                                             const std::string& sip_authorization) :
                                             Diameter::Message(dict, dict->MULTIMEDIA_AUTH_REQUEST)
{
  add_new_session_id();
  add_vendor_spec_app_id();
  add(Diameter::AVP(dict->AUTH_SESSION_STATE).val_i32(1));
  add(Diameter::AVP(dict->DESTINATION_REALM).val_str(dest_realm));
  add(Diameter::AVP(dict->DESTINATION_HOST).val_str(dest_host));
  add_origin();
  add(Diameter::AVP(dict->USER_NAME).val_str(impi));
  add(Diameter::AVP(dict->PUBLIC_IDENTITY).val_str(impu));
  Diameter::AVP sip_auth_data_item(dict->SIP_AUTH_DATA_ITEM);
  sip_auth_data_item.add(Diameter::AVP(dict->SIP_AUTH_SCHEME).val_str(sip_auth_scheme));
  if (!sip_authorization.empty())
  {
    sip_auth_data_item.add(Diameter::AVP(dict->SIP_AUTHORIZATION).val_str(sip_authorization));
  }
  add(sip_auth_data_item);
  add(Diameter::AVP(dict->SIP_NUMBER_AUTH_ITEMS).val_i32(1));
  add(Diameter::AVP(dict->SERVER_NAME).val_str(server_name));
}

std::string MultimediaAuthRequest::impu() const
{

  return get_str_from_avp(dict()->USER_NAME);
}

MultimediaAuthAnswer::MultimediaAuthAnswer(const Dictionary* dict,
                                           int result_code) :
                                           Diameter::Message(dict, dict->MULTIMEDIA_AUTH_ANSWER)
{
  add(Diameter::AVP(dict->RESULT_CODE).val_i32(result_code));
}

int MultimediaAuthAnswer::result_code() const
{
  return get_result_code();
}

int MultimediaAuthAnswer::experimental_result_code() const
{
  return get_experimental_result_code();
}

std::string MultimediaAuthAnswer::sip_auth_scheme() const
{
  std::string sip_auth_scheme;
  Diameter::AVP::iterator avps = begin(((Cx::Dictionary*)dict())->SIP_AUTH_DATA_ITEM);
  if (avps != end())
  {
    avps = avps->begin(((Cx::Dictionary*)dict())->SIP_AUTH_SCHEME);
    if (avps != end())
    {
      sip_auth_scheme = avps->val_str();
    }
  }
  return sip_auth_scheme;
}

DigestAuthVector MultimediaAuthAnswer::digest_auth_vector() const
{
  DigestAuthVector digest_auth_vector;
  Diameter::AVP::iterator avps = begin(((Cx::Dictionary*)dict())->SIP_AUTH_DATA_ITEM);
  if (avps != end())
  {
    avps = avps->begin(((Cx::Dictionary*)dict())->SIP_DIGEST_AUTHENTICATE);
    if (avps != end())
    {
      // Look for the digest.
      Diameter::AVP::iterator avps2 = avps->begin(((Cx::Dictionary*)dict())->CX_DIGEST_HA1);
      if (avps2 != end())
      {
        digest_auth_vector.ha1 = avps2->val_str();
      }
      else
      {
        // Some HSSs (in particular OpenIMSCore), use non-3GPP Digest-HA1.  Check for this too.
        avps2 = avps->begin(((Cx::Dictionary*)dict())->DIGEST_HA1);
        if (avps2 != end())
        {
          digest_auth_vector.ha1 = avps2->val_str();
        }
      }
      // Look for the realm.
      avps2 = avps->begin(((Cx::Dictionary*)dict())->CX_DIGEST_REALM);
      if (avps2 != end())
      {
        digest_auth_vector.realm = avps2->val_str();
      }
      else
      {
        // Some HSSs (in particular OpenIMSCore), use non-3GPP Digest-Realm.  Check for this too.
        avps2 = avps->begin(((Cx::Dictionary*)dict())->DIGEST_REALM);
        if (avps2 != end())
        {
          digest_auth_vector.realm = avps2->val_str();
        }
      }
      // Look for the QoP.
      avps2 = avps->begin(((Cx::Dictionary*)dict())->CX_DIGEST_QOP);
      if (avps2 != end())
      {
        digest_auth_vector.qop = avps2->val_str();
      }
      else
      {
        // Some HSSs (in particular OpenIMSCore), use non-3GPP Digest-QoP.  Check for this too.
        avps2 = avps->begin(((Cx::Dictionary*)dict())->DIGEST_QOP);
        if (avps2 != end())
        {
          digest_auth_vector.qop = avps2->val_str();
        }
      }
    }
  }
  return digest_auth_vector;
}

AKAAuthVector MultimediaAuthAnswer::aka_auth_vector() const
{
  AKAAuthVector aka_auth_vector;
  Diameter::AVP::iterator avps = begin(((Cx::Dictionary*)dict())->SIP_AUTH_DATA_ITEM);
  if (avps != end())
  {
    // Look for the challenge.
    Diameter::AVP::iterator avps2 = avps->begin(((Cx::Dictionary*)dict())->SIP_AUTHENTICATE);
    if (avps2 != end())
    {
      size_t len;
      const uint8_t* data = avps2->val_os(len);
      aka_auth_vector.challenge = base64(data, len);
    }
    // Look for the response.
    avps2 = avps->begin(((Cx::Dictionary*)dict())->SIP_AUTHORIZATION);
    if (avps2 != end())
    {
      size_t len;
      const uint8_t* data = avps2->val_os(len);
      aka_auth_vector.response = hex(data, len);
    }
    // Look for the encryption key.
    avps2 = avps->begin(((Cx::Dictionary*)dict())->CONFIDENTIALITY_KEY);
    if (avps2 != end())
    {
      size_t len;
      const uint8_t* data = avps2->val_os(len);
      aka_auth_vector.crypt_key = hex(data, len);
    }
    // Look for the integrity key.
    avps2 = avps->begin(((Cx::Dictionary*)dict())->INTEGRITY_KEY);
    if (avps2 != end())
    {
      size_t len;
      const uint8_t* data = avps2->val_os(len);
      aka_auth_vector.integrity_key = hex(data, len);
    }
  }
  return aka_auth_vector;
}

std::string MultimediaAuthAnswer::hex(const uint8_t* data, size_t len)
{
  static const char* const hex_lookup = "0123456789abcdef";
  std::string result;
  result.reserve(2 * len);
  for (size_t ii = 0; ii < len; ++ii)
  {
    const uint8_t b = data[ii];
    result.push_back(hex_lookup[b >> 4]);
    result.push_back(hex_lookup[b & 0x0f]);
  }
  return result;
}

std::string MultimediaAuthAnswer::base64(const uint8_t* data, size_t len)
{
  std::stringstream os;
  std::copy(boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<const uint8_t*,6,8> >(data),
            boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<const uint8_t*,6,8> >(data + len),
            boost::archive::iterators::ostream_iterator<char>(os));
  return os.str();
}

ServerAssignmentRequest::ServerAssignmentRequest(const Dictionary* dict,
                                                 const std::string& dest_host,
                                                 const std::string& dest_realm,
                                                 const std::string& impi,
                                                 const std::string& impu,
                                                 const std::string& server_name) :
                                                 Diameter::Message(dict, dict->SERVER_ASSIGNMENT_REQUEST)
{
  add_new_session_id();
  add_vendor_spec_app_id();
  add(Diameter::AVP(dict->AUTH_SESSION_STATE).val_i32(1));
  add_origin();
  add(Diameter::AVP(dict->DESTINATION_HOST).val_str(dest_host));
  add(Diameter::AVP(dict->DESTINATION_REALM).val_str(dest_realm));
  if (!impi.empty())
  {
    add(Diameter::AVP(dict->USER_NAME).val_str(impi));
  }
  add(Diameter::AVP(dict->PUBLIC_IDENTITY).val_str(impu));
  add(Diameter::AVP(dict->SERVER_NAME).val_str(server_name));
  if (!impi.empty())
  {
    add(Diameter::AVP(dict->SERVER_ASSIGNMENT_TYPE).val_i32(1));
  }
  else
  {
    add(Diameter::AVP(dict->SERVER_ASSIGNMENT_TYPE).val_i32(3));
  }
  add(Diameter::AVP(dict->USER_DATA_ALREADY_AVAILABLE).val_i32(0));
}

ServerAssignmentAnswer::ServerAssignmentAnswer(const Dictionary* dict) :
                                               Diameter::Message(dict, dict->SERVER_ASSIGNMENT_ANSWER)
{
}

int ServerAssignmentAnswer::result_code() const
{
  return get_result_code();
}

int ServerAssignmentAnswer::experimental_result_code() const 
{
  return get_experimental_result_code();
}

std::string ServerAssignmentAnswer::user_data() const
{
  return get_str_from_avp(((Cx::Dictionary*)dict())->USER_DATA);
}