# dde-network-utils

dde-network-utils 是对 dde-daemon 的网络模块所提供的 DBus 数据的一个再封装。由于原始 DBus 数据中存在许多信息杂糅、经常要处理字符串及 Json 数据解析等问题，再加上网络模块涉及有设备信息、代理信息、网络连接信息等许多信息的处理与聚集关联，单一只靠 DBus 已经无法高效、合理的处理它们。同时由于 DBus 调用固有的延迟及低可靠性等问题，也需要有一个中间层来作为缓冲。

# 基本模块
- NetworkModel 作为网络信息的数据模型，提供网络数据的 getter 方法与状态通知信号。
- NetworkWorker 作为网络后端的抽象，提供网络控制动作的 setter 方法，并主动同步数据到 model。
- NetworkDevice 作为网络设备的抽象。
    - WiredDevice 有线网络设备
    - WirelessDevice 无线网络设备

## NetworkModel
NetworkModel 是上层应用访问网络数据的第一入口，它保存了网络相关的所有信息，主要有两个方面：

- 全局网络信息
    - 全局网络状态
    - 网络代理状态
    - 设备无关的网络连接状态
- 网络设备列表
    - 有线设备及其关联的连接列表
    - 无线设备及其关联的 AP 列表

NetworkModel 类的主要公共接口提供了设备列表和各个设备无关的网络属性的获取，并实现了这些属性在变化时的 changed 信号。其中，设备相关的属性及信息被分发到了各个设备中进行具体处理，全局及公用的一些属性放在本类中管理。

由于网络模块的复杂性及后端接口的局限，目前网络模块需要小心的一点是不同类型信息之前的聚合处理，例如如何将网络连接、已激活的网络连接、有线及无线设备信息之间相互关联进来。**每个网络连接即对应着一个真实的 NetworkManager 的配置文件，它的唯一字段是 UUID**，注意这里说的网络连接不要和无线设备中AccessPoint的概念混淆。而同时在设备中，每个设备也有自己的 UUID。但需要注意的是，在目前的后端数据模型中，UUID 仅仅只能用来匹配网络连接和已激活的网络连接，如果想知道设备上的连接信息，需要通过设备的 MAC 地址和配置文件中所关联的 MAC 地址做比对。此处还需要注意的是设备的 MAC 地址分为克隆 MAC 地址和真实 MAC 地址。

## NetworkWorker
NetworkWorker 是上层业务逻辑实现网络控制的唯一途径。底层通过 DBus 调用 dde-daemon 所提供的网络配置接口。本类主要是进行了接口抽象，并做了一些基本的预处理以方便上层业务进行网络控制。同时，handle 了来自后端的数据变化，在进行简单的加工后注入到 NetworkModel 里并通过 NetworkModel 的 notify 事件向上传递。

NetworkWorker 中有一类 queryXxx 的方法，用来提供给上层业务一个主动刷新各种数据的方法，最终这些 queryXxx 方法会被转化成底层 DBus 调用执行，结果会注入回 NetworkModel 中，通过 notify 事件再反馈到 UI 层。

需要注意的是，在正常的业务逻辑过程中，queryXxx 是不应该出现的，NetworkModel 会自动处理所有动态信息，这些接口应该只在界面初始化刷新时，可能需要被调用一次。

## NetworkDevice
NetworkDevice 是对网络设备的一个基本抽象，作为设备管理的基本单元。一些与设备相关的属性和其对应的处理信号被封装在了设备内部，以提供更好的 OOP 接口。

NetworkDevice 对应了机器上的每个真实网络设备，它具有自己的基本硬件信息和网络状态信息，提供了此网卡的工作状态信息及状态转换信号。

### WiredDevice
由 NetworkDevice 派生出来的有线网络设备，增加了有线网络特有的 ActiveConnection 等属性。

### WirelessDevice
由 NetworkDevice 派生出来的无线网络设备，增加了如热点信息、AP 列表信息等无线网络特有的信息。
在WirelessDevice中，网络连接和无线网络连接的概念都有，那么同时，已激活网络连接(ActiveConnection)和已激活无线网络连接(ActiveApInfo)的概念也有，需要注意区分。一个网络连接如`NetworkModel`一节中所介绍的那样代表了NetworkManager的一个配置文件，而无线网络连接则**没有**对应的NetworkManager的配置文件。另外每一个网络连接可以提供UUID，ipv4，ipv6，网关等信息，每一个无线网络连接可以提供path(唯一标识)，ssid(无线网名称)，secret(是否加密)，strength(信号强度)等信息。

# Roadmap
dde-daemon 所提供的后端逻辑已经有些过时了，将来可以尝试直接连接 NetworkManager 的 DBus 接口进行网络控制，通过重构 NetworkWorker 或者添加新的抽象层来将后端接口迁移到更底层的控制逻辑上。
