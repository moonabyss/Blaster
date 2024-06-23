// Blaster Multiplayer Game. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class BlasterTarget : TargetRules
{
    public BlasterTarget(TargetInfo Target)
        : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V4;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        ExtraModuleNames.AddRange(new string[] { "Blaster" });
    }
}
