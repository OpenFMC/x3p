/***************************************************************************
 *   Copyright by Georg Wiora (NanoFocus AG) 2008                          *
 *                                                                         *
 *   This file is part of the openGPS (R)[TM] software library.            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License (LGPL)    *
 *   as published by the Free Software Foundation; either version 3 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the files "licence_LGPL-3.0.txt" and                   *
 *   "licence_GPL-3.0.txt".                                                *
 *                                                                         *
 *   openGPS is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 *   The name "openGPS" and the logo are registered as                     *
 *   European trade mark No. 006178354 for                                 *
 *   Physikalisch Technische Bundesanstalt (PTB)                           *
 *   http://www.ptb.de/                                                    *
 *                                                                         *
 *   More information about openGPS can be found at                        *
 *   http://www.opengps.eu/                                                *
 ***************************************************************************/

// This file does not contain compilable code, but the doxigen descriptions for the sample files

/// @defgroup X3P_Sample_Files Sample files demonstrating X3P file format.
/*@{*/

/*! @file ISO5436_2_XML_Demo/samplefiles/ISO5436-sample1.x3p
    \brief Simple matrix example with documented XML-main file.

    This file gives a very simple example with a fully documented xml-main file.
    The profile values are stored in a 4 by 4 by 1 matrix.
    To view the contents of the file, extract it by using any unzip-tool.
    The following files are contained in this container:
    
    \li main.xml The main document xml-file.
    \li md5checksum.hex The MD5 checksum of the main document.

*/

/*! @file ISO5436_2_XML_Demo/samplefiles/ISO5436-sample1_bin.x3p
    \brief Simple binary matrix example with documented XML-main file.

    This file contains the same data as ISO5436-sample1.x3p but uses
    binary encoding for the profile data.
    To view the contents of the file, extract it by using any unzip-tool.
    The following files are contained in this container:

    \li main.xml The main document xml-file.
    \li md5checksum.hex The MD5 checksum of the main document.
    \li bindata/data.bin Binary encoded profile data
    \li bindata/valid.bin Validity matrix for profile data
*/

/*! @file ISO5436_2_XML_Demo/samplefiles/ISO5436-sample2.x3p
    \brief Simple unordered list example with documented XML-main file.

    This file gives a very simple example with a fully documented xml-main file.
    The 4 by 4 profile values are stored in an unordered list.
    To view the contents of the file, extract it by using any unzip-tool.

    \li main.xml The main document xml-file.
    \li md5checksum.hex The MD5 checksum of the main document.

*/

/*! @file ISO5436_2_XML_Demo/samplefiles/ISO5436-sample3.x3p
    \brief Matrix example mixed incremental/absolute axis.

    This file gives a little more complicate example with a fully documented xml-main file.
    The 4 by 4 profile values are stored with an incremental x-axis 
    an absolute integer y axis and an absolute double z-axis as a 4 by 4 by 1 matrix.
    To view the contents of the file, extract it by using any unzip-tool.

    \li main.xml The main document xml-file.
    \li md5checksum.hex The MD5 checksum of the main document.

*/

/*! @file ISO5436_2_XML_Demo/samplefiles/ISO5436-sample4.x3p
    \brief Matrix example with integer encoded z-values.

    This file gives a little more complicate example with a fully documented xml-main file.
    The 4 by 4 profile values are stored with an incremental x- and y-axis.
    The z-axis is integer encoded.
    To view the contents of the file, extract it by using any unzip-tool.

    \li main.xml The main document xml-file.
    \li md5checksum.hex The MD5 checksum of the main document.

*/

/*! @file ISO5436_2_XML_Demo/samplefiles/ISO5436-sample4_bin.x3p
    \brief Matrix example with integer encoded z-values in binary format.

    This file contains the same data as ISO5436-sample4.x3p but with binary encoded profile data.
    The 4 by 4 profile values are stored with an incremental x- and y-axis.
    The z-axis is integer encoded.
    To view the contents of the file, extract it by using any unzip-tool.

    \li main.xml The main document xml-file.
    \li md5checksum.hex The MD5 checksum of the main document.
    \li bindata/data.bin Binary encoded profile data
    \li bindata/valid.bin Validity matrix for profile data
*/

/*@}*/
