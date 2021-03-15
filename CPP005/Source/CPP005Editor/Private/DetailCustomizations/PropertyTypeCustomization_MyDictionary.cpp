#include "PropertyTypeCustomization_MyDictionary.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "SGraphPinComboBox.h"
#include "IPropertyUtilities.h"
#include "MyDictionary.h"

#define LOCTEXT_NAMESPACE "PropertyTypeCustomization_MyDictionary"

TSharedRef<IPropertyTypeCustomization> FPropertyTypeCustomization_MyDictionary::MakeInstance() {
	return MakeShareable(new FPropertyTypeCustomization_MyDictionary);
}

void FPropertyTypeCustomization_MyDictionary::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) {

	PropertyHandlePtr = PropertyHandle;
	TSharedPtr<IPropertyUtilities> PropertyUtils = CustomizationUtils.GetPropertyUtilities();

	// Get list of MyDictionary indexes
	TArray< TSharedPtr<int32> > ComboItems;
	GenerateComboBoxIndexes(ComboItems);

	ComboBox = SNew(SPinComboBox)
		.ComboItemList(ComboItems)
		.VisibleText(this, &FPropertyTypeCustomization_MyDictionary::OnGetText)
		.OnSelectionChanged(this, &FPropertyTypeCustomization_MyDictionary::ComboBoxSelectionChanged)
		.OnGetDisplayName(this, &FPropertyTypeCustomization_MyDictionary::OnGetFriendlyName)
		.OnGetTooltip(this, &FPropertyTypeCustomization_MyDictionary::OnGetTooltip);

	HeaderRow.NameContent()[PropertyHandle->CreatePropertyNameWidget()]
		.ValueContent()[ComboBox.ToSharedRef()].IsEnabled(MakeAttributeLambda([=] { return !PropertyHandle->IsEditConst() && PropertyUtils->IsPropertyEditingEnabled(); }));
}

void FPropertyTypeCustomization_MyDictionary::GenerateComboBoxIndexes(TArray< TSharedPtr<int32> >& OutComboBoxIndexes) {
	int32 i = 0;
	for (auto item : FMyDictionary::Items) {
		TSharedPtr<int32> EnumIdxPtr(new int32(i++));
		OutComboBoxIndexes.Add(EnumIdxPtr);
	}
}

FString FPropertyTypeCustomization_MyDictionary::OnGetText() const {

	if (auto myDictionary = GetPropertyAs<FMyDictionary>()) {

		auto itemIndex = myDictionary->ItemIndex;
		
		return (FMyDictionary::Items.Num() < itemIndex || itemIndex < 0)
			? ""
			: FMyDictionary::Items[itemIndex].ToString();
	}

	return "";
}

void FPropertyTypeCustomization_MyDictionary::ComboBoxSelectionChanged(TSharedPtr<int32> NewSelection, ESelectInfo::Type /*SelectInfo*/) {
	//if (NewSelection.IsValid()) {

	//	FStructProperty* StructProperty = CastField<FStructProperty>(StructPropertyHandle->GetProperty());

	//	if (auto replica = GetPropertyAs<FReplica>()) {
	//		replica->Index = *NewSelection;
	//	}
	//}
}

FText FPropertyTypeCustomization_MyDictionary::OnGetFriendlyName(int32 itemIndex) {
	return (FMyDictionary::Items.Num() < itemIndex || itemIndex < 0)
		? FText::GetEmpty()
		: FText::FromName(FMyDictionary::Items[itemIndex]);
}

FText FPropertyTypeCustomization_MyDictionary::OnGetTooltip(int32 itemIndex) {
	return (FMyDictionary::Items.Num() < itemIndex || itemIndex < 0)
		? FText::GetEmpty()
		: FText::FromName(FMyDictionary::Items[itemIndex]);
}

#undef LOCTEXT_NAMESPACE