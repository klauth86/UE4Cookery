#include "PropertyTypeCustomization_MyDictionary.h"

#define LOCTEXT_NAMESPACE "PropertyTypeCustomization_MyDictionary"

TSharedRef<IPropertyTypeCustomization> FPropertyTypeCustomization_MyDictionary::MakeInstance() {
	return MakeShareable(new FPropertyTypeCustomization_MyDictionary);
}

void FPropertyTypeCustomization_MyDictionary::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) {

}

#undef LOCTEXT_NAMESPACE