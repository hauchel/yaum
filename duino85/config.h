// This file is part of cryptosuite2.                                    //
//                                                                       //
// cryptosuite2 is free software: you can redistribute it and/or modify  //
// it under the terms of the GNU General Public License as published by  //
// the Free Software Foundation, either version 3 of the License, or     //
// (at your option) any later version.                                   //
//                                                                       //
// cryptosuite2 is distributed in the hope that it will be useful,       //
// but WITHOUT ANY WARRANTY; without even the implied warranty of        //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
// GNU General Public License for more details.                          //
//                                                                       //
// You should have received a copy of the GNU General Public License     //
// along with cryptosuite2.  If not, see <http://www.gnu.org/licenses/>. //
//                                                                       //



// include the module config first, 
// overwrite it in the arduino interface config.
#include "sha1/default.h"

#ifndef SHA_CONFIG_H_
#define SHA_CONFIG_H_


//changes as recommended.
#define SHA256_DISABLED
#undef  SHA256_ENABLE_HMAC
#undef  SHA1_ENABLE_HMAC
#define SHA256_DISABLE_WRAPPER


#endif
