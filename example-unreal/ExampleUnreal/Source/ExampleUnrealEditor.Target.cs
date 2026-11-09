using UnrealBuildTool;

public class ExampleUnrealEditorTarget : TargetRules
{
	public ExampleUnrealEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V8;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_9;
		ExtraModuleNames.Add("ExampleUnreal");
	}
}
