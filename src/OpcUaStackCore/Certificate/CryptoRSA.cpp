/*
   Copyright 2018 Kai Huebl (kai@huebl-sgh.de)

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

#include <openssl/rsa.h>
#include "OpcUaStackCore/Certificate/CryptoRSA.h"

namespace OpcUaStackCore
{

	CryptoRSA::CryptoRSA(void)
	: OpenSSLError()
	, isLogging_(false)
	{
	}

	CryptoRSA::~CryptoRSA(void)
	{
	}

	void
	CryptoRSA::isLogging(bool isLogging)
	{
		isLogging_ = isLogging;
	}

	bool
	CryptoRSA::isLogging(void)
	{
		return isLogging_;
	}

	OpcUaStatusCode
	CryptoRSA::publicEncrypt(
	    char*      plainTextBuf,
	    uint32_t   plainTextLen,
	    PublicKey* publicKey,
	    int16_t    padding,
	    char*      encryptedTextBuf,
	    int32_t*   encryptedTextLen
	)
	{
		*encryptedTextLen = 0;

		// check plain text
		if (plainTextBuf == nullptr || plainTextLen == 0) {
			if (isLogging_) {
				Log(Error, "publicEntrypt error - plainTextBuf is empty");
			}
			return BadInvalidArgument;
		}

		// check public key
		if (publicKey->keyType() != KeyType_RSA) {
			if (isLogging_) {
				Log(Error, "publicEntrypt error - invalid public key type");
			}
			return BadInvalidArgument;
		}

		// get key information
		EVP_PKEY* key = *publicKey;
		if (key == nullptr) {
			if (isLogging_) {
				Log(Error, "publicEntrypt error - key is empty");
			}
			return BadUnexpectedError;
		}

		if (key->pkey.rsa == nullptr) {
			if (isLogging_) {
				Log(Error, "publicEntrypt error - invalid rsa key type");
			}
			return BadUnexpectedError;
		}
		uint32_t keySize = publicKey->keySize();

	    // check padding type
		uint32_t encryptedDataSize  = 0;
	    switch(padding)
	    {
	    	case RSA_PKCS1_PADDING:
	        {
	        	encryptedDataSize = keySize - 11;
	            break;
	        }
	    	case RSA_PKCS1_OAEP_PADDING:
	        {
	        	encryptedDataSize = keySize - 42;
	            break;
	        }
	    	case RSA_NO_PADDING:
	        {
	        	encryptedDataSize = keySize;
	            break;
	        }
	    	default:
	        {
				if (isLogging_) {
					Log(Error, "publicEntrypt error - pinvalid padding type");
				}
	        	EVP_PKEY_free(key);
	            return BadNotSupported;
	        }
	    }

	    // check bytes to encrypt in one step
	    uint32_t bytesToEncrypt = 0;
	    if(plainTextLen < encryptedDataSize) {
	    	bytesToEncrypt = plainTextLen;
	    }
	    else {
	    	bytesToEncrypt = encryptedDataSize;
	    }

	    // encrypt
	    uint32_t plainTextPosition = 0;
	    uint32_t encryptedTextPosition = 0;
	    while(plainTextPosition < plainTextLen) {
	    	int32_t encryptedBytes;

	    	// the last part could be smaller
	    	if((plainTextLen >= encryptedDataSize) && ((plainTextLen - plainTextPosition) < encryptedDataSize)) {
	            bytesToEncrypt = plainTextLen - plainTextPosition;
	        }

	    	if((encryptedTextBuf != nullptr) && (plainTextBuf != nullptr)) {
	    		// encrypt buffer
	    		encryptedBytes = RSA_public_encrypt(
	    		    bytesToEncrypt,      	   										// number bytes to encrypt
	    			(const unsigned char*)(plainTextBuf + plainTextPosition),       // buffer to encrypt
	    			(unsigned char*)(encryptedTextBuf + encryptedTextPosition),  	// where to encrypt
	    			key->pkey.rsa,                              					// public key
	    			padding															// padding mode
			    );
	    		if(encryptedBytes < 0) {
	    			addOpenSSLError();

	    			EVP_PKEY_free(key);
	    			if (isLogging_) {
	    				log(Error, "publicEntrypt error - RSA_public_encrypt");
	    			}
	    			return BadUnexpectedError;
	    		}
	    	}
	    	else {
	    		encryptedBytes = keySize;
	    	}

	    	*encryptedTextLen = *encryptedTextLen + encryptedBytes;
	    	encryptedTextPosition += keySize;
	    	plainTextPosition  += bytesToEncrypt;
	    }

	    EVP_PKEY_free(key);
		return Success;
	}

	OpcUaStatusCode
	CryptoRSA::privateDecrypt(
		char*       encryptedTextBuf,
		uint32_t    encryptedTextLen,
		PrivateKey* privateKey,
		int16_t     padding,
	    char*       plainTextBuf,
	    int32_t*    plainTextLen
	)
	{
		*plainTextLen = 0;

		// check public key
		if (privateKey->keyType() != KeyType_RSA) {
			if (isLogging_) {
				Log(Error, "privateDecrypt error - invalid private key type");
			}
			return BadInvalidArgument;
		}

		// get private key
		EVP_PKEY* key = *privateKey;
	    if (key == nullptr) {
			if (isLogging_) {
				Log(Error, "privateDecrypt error - key empty");
			}
	        return BadUnexpectedError;
	    }
		if (key->pkey.rsa == nullptr) {
			if (isLogging_) {
				Log(Error, "privateDecrypt error - private rsa key empty");
			}
			return BadUnexpectedError;
		}

		// get key information
		uint32_t keySize = RSA_size(key->pkey.rsa);

		// check encrypted text
		if ((encryptedTextLen == 0) || (encryptedTextLen%keySize != 0)) {
			if (isLogging_) {
				Log(Error, "privateDecrypt error - encrypted text length");
			}
			return BadInvalidArgument;
		}

		// check padding type
		uint32_t decryptedDataSize = 0;
	    switch(padding)
	    {
	    	case RSA_PKCS1_PADDING:
	        {
	        	decryptedDataSize = keySize - 11;
	            break;
	        }
	    	case RSA_PKCS1_OAEP_PADDING:
	        {
	        	decryptedDataSize = keySize - 42;
	            break;
	        }
	    	case RSA_NO_PADDING:
	        {
	        	decryptedDataSize = keySize;
	            break;
	        }
	    	default:
	        {
				if (isLogging_) {
					Log(Error, "privateDecrypt error - invalid padding");
				}
	            return BadNotSupported;
	        }
	    }

	    uint32_t decryptedBytes = 0;
		uint32_t encryptedTextPosition = 0;
		while (encryptedTextPosition < encryptedTextLen) {

			if (plainTextBuf != nullptr) {
				decryptedBytes = RSA_private_decrypt(
					keySize,                            							// how much to decrypt
					(unsigned char*)encryptedTextBuf + encryptedTextPosition,   	// what to decrypt
					(unsigned char*)plainTextBuf + (*plainTextLen),  				// where to decrypt
					key->pkey.rsa,                  								// private key
					padding															// padding mode
				);
				if(decryptedBytes == -1) {
					addOpenSSLError();

					if (isLogging_) {
						log(Error, "privateDecrypt error - RSA_private_decrypt");
					}
					EVP_PKEY_free(key);
					return BadUnexpectedError;
				}

			}
			else {
				decryptedBytes = decryptedDataSize;
			}

			*plainTextLen = *plainTextLen + decryptedBytes;
			encryptedTextPosition = encryptedTextPosition + keySize;
		}

		EVP_PKEY_free(key);

		return Success;
	}

	OpcUaStatusCode
	CryptoRSA::privateSign(
		char*       plainTextBuf,
		uint32_t    plainTextLen,
		PrivateKey* privateKey,
	    int32_t     digest,
	    char*       signTextBuf,
	    uint32_t*   signTextLen
	)
	{
		// check public key
		if (privateKey->keyType() != KeyType_RSA) {
			if (isLogging_) {
				Log(Error, "sign error - invalid private key type");
			}
			return BadInvalidArgument;
		}

		// get private key
		EVP_PKEY* key = (*privateKey);
	    if (key == nullptr) {
			if (isLogging_) {
				Log(Error, "sign error - private key empty");
			}
	        return BadUnexpectedError;
	    }
		if (key->pkey.rsa == nullptr) {
			if (isLogging_) {
				Log(Error, "sign error - private rsa key empty");
			}
			return BadUnexpectedError;
		}

		// check sign text length
		uint32_t size =  RSA_size(key->pkey.rsa);
		if (size != *signTextLen) {
			if (isLogging_) {
				Log(Error, "sign error - sign text length");
			}
			return BadInvalidArgument;
		}

		// sign plain text
		*signTextLen = 0;
		int32_t result = RSA_sign(
			digest,
			(const unsigned char*)plainTextBuf,
			plainTextLen,
			(unsigned char*)signTextBuf,
			signTextLen,
			key->pkey.rsa
		);
		if (result != 1) {
			addOpenSSLError();

			if (isLogging_) {
				log(Error, "sign error - RSA_sign");
			}
			return BadUnexpectedError;
		}

		return Success;
	}

	OpcUaStatusCode
	CryptoRSA::publicVerify(
		char*       plainTextBuf,
		uint32_t    plainTextLen,
		PublicKey*  publicKey,
	    int32_t     digest,
	    char*       signTextBuf,
	    uint32_t    signTextLen
	)
	{
		// check public key
		if (publicKey->keyType() != KeyType_RSA) {
			if (isLogging_) {
				Log(Error, "sign error - public key type error");
			}
			return BadInvalidArgument;
		}

		// get key information
		EVP_PKEY* key = *publicKey;
		if (key == nullptr) {
			if (isLogging_) {
				Log(Error, "sign error - public key is empty");
			}
			return BadUnexpectedError;
		}

		if (key->pkey.rsa == nullptr) {
			if (isLogging_) {
				Log(Error, "sign error - public rsa key type error");
			}
			return BadUnexpectedError;
		}

		// check sign text length
		uint32_t size =  RSA_size(key->pkey.rsa);
		if (signTextLen % size != 0) {
			if (isLogging_) {
				Log(Error, "sign error - sign text length error");
			}
			return BadInvalidArgument;
		}

		int32_t result = RSA_verify(
			digest,
			(const unsigned char*)plainTextBuf,
			plainTextLen, (unsigned char *)signTextBuf,
			signTextLen,
			key->pkey.rsa
		);
		if (result != 1) {
			addOpenSSLError();

			if (isLogging_) {
				log(Error, "sign error - RSA_verify");
			}
		    return BadSignatureInvalid;
		}

		return Success;
	}

}
