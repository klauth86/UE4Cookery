#pragma once

#include "IPropertyTypeCustomization.h"

class SPinComboBox;

class CPP005EDITOR_API FPropertyTypeCustomization_MyDictionary : public IPropertyTypeCustomization {

public:

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override {}

protected:

	void GenerateComboBoxIndexes(TArray< TSharedPtr<int32> >& OutComboBoxIndexes);

	FString OnGetText() const;

	void ComboBoxSelectionChanged(TSharedPtr<int32> NewSelection, ESelectInfo::Type SelectInfo);

	FText OnGetFriendlyName(int32 itemIndex);

	FText OnGetTooltip(int32 itemIndex);

	template<typename T>
	T* GetPropertyAs() const {
		if (PropertyHandlePtr.IsValid()) {
			TArray<void*> RawData;
			PropertyHandlePtr->AccessRawData(RawData);
			return reinterpret_cast<T*>(RawData[0]);
		}

		return nullptr;
	}

protected:

	TSharedPtr<IPropertyHandle> PropertyHandlePtr;

	TSharedPtr<SPinComboBox> ComboBox;
};