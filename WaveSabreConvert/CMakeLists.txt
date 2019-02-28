add_library(WaveSabreConvert SHARED
	FLConverter.cs
	ILog.cs
	LiveConverter.cs
	LiveParser.cs
	LiveProject.cs
	MetaPlugin.cs
	ProjectConverter.cs
	Properties/AssemblyInfo.cs
	Properties/Resources.Designer.cs
	ReaperConverter.cs
	ReaperElements/ReaperElement.cs
	ReaperElements/ReaperProject.cs
	ReaperElements/ReaperTag.cs
	ReaperElements/ReaperTrack.cs
	ReaperParser.cs
	RenoiseConverter.cs
	RenoiseInject.cs
	RenoiseParser.cs
	RenoiseSong63.cs
	SendItConfig.cs
	Serializer.cs
	Song.cs
	SongDiff.cs
	Utils.cs)

set_target_properties(WaveSabreConvert PROPERTIES VS_DOTNET_REFERENCES "Microsoft.Build.Utilities.v4.0;Microsoft.CSharp;${CMAKE_CURRENT_SOURCE_DIR}/FLParser/Monad.FLParser.dll;System;System.Core;System.Data;System.Data.DataSetExtensions;System.IO.Compression;System.IO.Compression.FileSystem;System.Xml;System.Xml.Linq")
