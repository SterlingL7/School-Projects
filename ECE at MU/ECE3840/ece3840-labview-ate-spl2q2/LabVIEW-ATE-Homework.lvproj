<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="18008000">
	<Item Name="My Computer" Type="My Computer">
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="AFG2021-SineWave.vi" Type="VI" URL="../AFG2021-SineWave.vi"/>
		<Item Name="AFG2021-SourceVoltageUnit.vi" Type="VI" URL="../AFG2021-SourceVoltageUnit.vi"/>
		<Item Name="DMM4040-Measure-VAC.vi" Type="VI" URL="../DMM4040-Measure-VAC.vi"/>
		<Item Name="ReadDutInputVoltage.vi" Type="VI" URL="../ReadDutInputVoltage.vi"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="instr.lib" Type="Folder">
				<Item Name="Tektronix AFG 3000 Series.lvlib" Type="Library" URL="/&lt;instrlib&gt;/Tektronix AFG 3000 Series/Tektronix AFG 3000 Series.lvlib"/>
				<Item Name="Tektronix DMM40x0 Series.lvlib" Type="Library" URL="/&lt;instrlib&gt;/Tektronix DMM40x0 Series/Tektronix DMM40x0 Series.lvlib"/>
			</Item>
			<Item Name="vi.lib" Type="Folder">
				<Item Name="Error Cluster From Error Code.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Cluster From Error Code.vi"/>
			</Item>
		</Item>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
</Project>
