using UnrealBuildTool;
 
public class MounteaDocumentationSystemEditor : ModuleRules
{
	public MounteaDocumentationSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"CoreUObject", 
				"Engine", 
				"UnrealEd",
				"SlateCore",
				"EditorStyle"
			});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				"EditorStyle",
				
				"EditorStyle",
				"InputCore",
				
				"DeveloperSettings",
				
				"PropertyEditor",
				
				"MounteaDocumentationSystem"
			});
	}
}