/****************************************************************************
 * C++ tests
 *
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Ndless code.
 *
 * The Initial Developer of the Original Code is tangrs.
 * Portions created by the Initial Developer are Copyright (C) 2013
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): Olivier ARMAND <olivier.calc@gmail.com>.
 ****************************************************************************/

#include <os.h>

class test_class {
	public:
	test_class() {
			printf("test_class constructor called\n");
	}
	int class_method() {
			printf("test_class->class_method called\n");
			return 0;
	}
};

class test_class2 : test_class {
	public:
	test_class2() {
			printf("test_class2 constructor called\n");
	}
	int class_method() {
			printf("test_class2->class_method called\n");
			return 0;
	}
};

int main(void) {
	test_class *obj1 = new test_class;
	test_class2 *obj2 = new test_class2;
	obj1->class_method();
	obj2->class_method();
	delete obj1;
	delete obj2;
	return 0;
}
