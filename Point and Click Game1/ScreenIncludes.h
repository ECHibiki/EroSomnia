#pragma once
#include<chrono>
#include <list>

#include "Graphics.h"
#include "SFX.h"
#include "GenericGameTools.h"
#include "MouseIO.h"

#include "TextBox.h"


namespace boost {
	namespace serialization {
		template <class Archive, class T>
		void serialize(Archive & ar, boost::shared_ptr<T> & t, const unsigned int ver, int) {
			ar & t.px;
			ar & t.pn;
		}
	}
}