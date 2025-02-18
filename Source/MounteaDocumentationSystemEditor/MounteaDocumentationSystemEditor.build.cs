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
				"UnrealEd"
			});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				
				"EditorStyle",
				"InputCore",
				
				"MounteaDocumentationSystem"
			});
	}
}