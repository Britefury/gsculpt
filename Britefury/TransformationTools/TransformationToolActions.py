##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.UI.UIAction import UIAction


deformAction = UIAction( 'Tools.Deformations.DeformationMode.Deform', ( _( 'Tools' ), _( 'Deformations' ), _( 'Deformation mode' ), _( 'Deformation mode' ) ),
			_( 'Deformation mode' ), _( 'Deformation mode' ), 'w', _( 'Deformation mode' ) )
rotationAction = UIAction( 'Tools.Deformations.DeformationMode.Rotation', ( _( 'Tools' ), _( 'Deformations' ), _( 'Deformation mode' ), _( 'Rotation mode' ) ),
			_( 'Rotation mode' ), _( 'Rotation mode' ), 'r', _( 'Rotation mode' ) )
