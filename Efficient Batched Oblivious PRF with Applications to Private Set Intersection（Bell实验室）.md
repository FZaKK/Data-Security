Efficient Batched Oblivious PRF with Applications to Private Set Intersection

PRF：伪随机函数（密码学）

OPRF：伪随机函数，发送者持有随机数种子K，接收者发送数据并接收伪随机函数F的输出结果，双方不知道对方的信息

oblivious  evaluation：应该是把PRF扩展为OPRF进行的不经意求值过程吧，oblivious evaluation，感觉是这样。

1-  out-of-2 OT-extension protocols：2选1 OT协议扩展，一般一种是长度扩展，另一种是使用\lamda个基础OT协议完成m个实际上的OT协议内容传输

PSI：隐私集合求交，可以有两方的隐私集合求交，也可以完成多方隐私集合求交

我们的OPRF可以用来消除他们的PSI协议对各方项目比特长度的依赖。？？

IKNP OT扩展协议：这里是使用\lamda个基础OT协议完成m个实际上的OT协议内容传输的一种扩展方法，实现的方法为将选择向量bit进行列扩展，接着通过选择列重构矩阵（生成消息对等离线阶段需要使用的数据）

GMW protocol：？？

许多安全计算的性能与效率都要依赖于batch OTs的低成本实现。

门陷秘密共享(GESS)

KK13协议：IKNP的基础上进行的n选1编码扩展
Kolesnikov和Kumaresan [KK13]根据GMW用例提出了对短秘密尺寸的改进。我们使用了他们协议中的想法，并将其称为KK协议。在背后，KK [KK13]注意到IKNP数据表示的一个核心方面可以抽象地视为重复错误纠正代码，它们的改进源于使用更好的代码。因此，可以以几乎相同的成本实现1 / n的OT，而不是1/2的OT, n高达约256。

---

**以下为论文学习部分**

在本篇论文之中，主要是提出了自己的轻量级OPRF（伪随机函数）以及以此为根据的创新PSI protocol，消除了原本的PSI协议之中对各方的item的bit长度的依赖，主要与Pinkas的PSI协议（Usenix 2015）进行了比较。

OPRF：OPRF的全称是Oblivious Pseudo-Random Function，即不经意伪随机函数。具体可参考学习：
[OPRF - PamShao - 博客园 (cnblogs.com)](https://www.cnblogs.com/pam-sh/p/16216240.html)
假设Alice有一些输入，Bob有一个key。OPRF允许Alice将自己的输入与Bob的key结合经过一系列运算转变成相对应的数。  
在这个过程中，Alice不能知道Bob的key，Bob也不知道最后的结果F(key, x<sub>i</sub>)。每一个输入x<sub>i</sub>都可以计算出一个不同于其他输入的数，这些数就可以被看作**伪随机数**。

IKNP：a surprisingly fast OT protocol （by Ishai)，对于IKNP的学习，可见数据安全教材的第三章的部分有详细的讲解（正在学习ing）

有个小问题：关于OT长度扩展以及Beaver的非黑盒构造部分的（具体在数据安全教材的第三章部分）
![[Pasted image 20230413095316.png]]
为什么，这里R接收方要向发送方S发送b异或G(r)呢，然后S再向函数F发送b异或G(r)，为啥不直接R接收方就给函数F提供r bit种子和b异或G(r)呢？？

Beaver非黑盒构造：使用伪随机函数，对OT长度进行扩展，因此只需要 用𝜆个（即常数个）OT 协议即可实现 m 个 OT 协议。基于 Beaver 的非黑盒构造可以满足 RR-OT 协议，也就是对RR-OT协议离线阶段每次只能生成一对2选1秘密选择的改进吧。

基于 Beaver 的非黑盒构造可以满足上述 RR-OT 协议，可以让 S 随机生成 m 对秘密值 (𝑥<sub>1</sub><sup>0</sup>,𝑥<sub>1</sub><sup>1</sup>), ⋯ , (𝑥<sub>m</sub><sup>0</sup> , 𝑥<sub>m</sub><sup>1</sup>  )，R 随机生成 m 比特长的选择比特串(𝑏<sub>1</sub>, ⋯ , 𝑏<sub>𝑚</sub>)即可，然后通过上述构造电路 C 的方式，即可让 S 获得 m 对秘密值、R 获得 m 对随机比特和对应的秘密。

Ishai 等人在 2003 年提出的 OT 扩展 **IKNP** 协议，此协议可在半诚实攻击者的攻击下实现 m 个 2 选 1 OT 协议，用来安全地传输 m 个随机字符串。IKNP 协议和 Beaver 等人解决的问题实际上是相同的，都是为了让 Alice 获得消息对(𝑚0, 𝑚1)和让Bob 获得对应的(𝑟, 𝑚𝑟)以便在线阶段 OT 的使用。

IKNP：将行数据利用列传递降低交互次数，这里的主要思想就是利用短一点的列长度来传递较长的选择bit，利用选择bit作为对应的列扩展的参考。
![[Pasted image 20230413105419.png]]
由此就已经可以明显的看出 Alice 拥有了随机的消息对，并且 Bob 拥有选择比特位和 Alice 消息对中的其中一个消息。这已解决开始想要解决的问题（两方获得用于在线阶段的消息对等）（类似于m<sub>1</sub>r，m<sub>2</sub>r）。

其实IKNP协议感觉也是基于RR-OT协议的流程的，只不过对于RR-OT协议而言，RR-OT协议每次在线阶段只能处理一对消息对，其中Beaver非黑盒构造是使用伪随机函数扩展选择bit，而IKNP则是使用矩阵之中的列扩展，再对列进行选择重构消息矩阵，来进行OT的扩展。对于Beaver非黑盒构造而言因为要处理m个消息的选择，输出到接收方，所以需要一个很大的乱码电路来对该方案进行求值（因为对于需要求值的函数F而言，还需要输入m对秘密值等内容），而对于IKNP协议而言则不需要使用混淆电路。

好像Beaver扩展和IKNP协议看懂了......

---

隐私计算研习社： 基于差分隐私的联邦学习数据隐私安全技术

主要针对联邦学习可能面临的安全和隐私威胁进行分析，着重阐述隐私安全问题，总结了一些防御措施。

基于**拉普拉斯机制**的差分隐私算法，设计了一个联邦学习的隐私保护方法？？？？

联邦学习利用多个计算节点进行联合训练，旨在提高性能、保护隐私信息，并使其可以扩展到更大规模的训练数据和更大规模的模型。
目前联邦学习系统中常用的聚合策略包括联邦平均（Federated Averaging，FedAvg）和联邦随机梯度下降（Federated Stochastic Gradient Descent，FedSGD）等。
一般地，基本的联邦学习系统至少包括初始化（分发全局模型）、本地更新和全局聚合 3 个步骤。

根据学习依据，联邦学习系统分为横向联邦学习、纵向联邦学习和联邦迁移学习 3 类。
横向联邦学习按照样本空间对数据集进行划分，并取出特征相同而样本不同的部分来进行训练。纵向联邦学习与横向联邦学习相反，是根据特征分布来进行学习的。联邦迁移学习适用于参与方的数据集中特征空间和样本空间重叠都很少的情况。联邦迁移学习中，不对数据进行切分，而是利用迁移学习的方法来解决数据及标签不足的问题。

联邦学习中的参与方往往容易受到模型提取攻击和模型逆向攻击这两大类攻击。

一般而言，模型提取攻击中，攻击者试图构建一个与原模型相似的替代模型。因为攻击者缺少对模型结构的了解，所以此类攻击一般为黑盒攻击。就是攻击者自己输入数据，获得模型输出的标签值，这些标签值被他当作数据集集中进行训练。

在模型逆向攻击中，攻击者一般试图通过在训练完毕的模型中不断地查询，获得一些统计信息，进而推理出用户的隐私信息。根据攻击者推理的信息，可以将模型逆向攻击分为属性推理攻击（Property Inference Attack，PIA）和成员推理攻击（Member Inference Attack，MIA）。

安全多方计算等内容是基于密码学算法的，另外有一些不基于密码学的隐私安全方法，例如差分隐私（Differential Privacy，DP），主要用于模型训练完毕后的隐私保护，其可以通过加噪的方法防御一些模型提取攻击和推理攻击。这些保护措施的安全性的高低主要体现在能否使得明文信息能够推理出尽量少的敏感信息。

邻近数据集（Adjacent Dataset）的定义。设 有两个数据集 D 与 D'，若它们的对称差中元素的数量满足![图片](https://mmbiz.qpic.cn/sz_mmbiz_png/iclynibMMTgBxDx1jVgRvx7Arf24icrgx5t4LiadDpMtN8OybZ9BDGPxMciboKibJpb5vLm7EOVTicejLBMPFf8xiaIWOg/640?wx_fmt=png&wxfrom=5&wx_lazy=1&wx_co=1)，则称 D 与 D' 互为邻近数据集。

![[Pasted image 20230413160257.png]]

其中参数ϵ 称为隐私预算，代表了差分隐私技术所实现的隐私保护程度，ϵ值越小表示隐私保护程度越高。Pr(M(D)∈S)表示随机算法M的输出属于集合S的概率。ϵ 表示违背严格差分隐私的概率，若δ = 0，则随机算法M表示严格的ϵ − 差分隐私，否则为松弛差分隐私。

常用的实现隐私保护的机制有拉普拉斯噪声机制和指数噪声机制。

为了获知对于特定的随机算法函数 M，需要添加多少噪声才能满足差分隐私，并且需要获知该算法在当前数据集中删除一条数据引起的查询结果的最大改变量。差分隐私算法有两种敏感度，分别是全局敏感度（Global Sensitivity）和局部敏感度（Local Sensitivity）。

设一映射
![[Pasted image 20230413201813.png]]
其输出为 d 维实向量，则全局敏感度定义如下：
![[Pasted image 20230413201858.png]]
全局敏感度为一个L<sub>1</sub>范数定义的表达式，由函数本身决定，而和 D 与 D' 无关。当全局敏感度较小时，只需要添加较小的噪声，就能使函数 M 满足差分隐私。

局部敏感度：
由于全局敏感度较大时，需要添加强度足够大的噪声才能保证隐私安全，这可能导致数据不可用，因此 Nissim 等人定义了局部敏感度，则 f 在数据集D 上的局部敏感度具体为：
![[Pasted image 20230413203336.png]]
可以发现局部敏感度与全局敏感度的关系如下：
![[Pasted image 20230413203355.png]]
局部敏感度通常比全局敏感度要小很多，并且局部敏感度取决于映射 f 本身与原始数据集 D，然而，局部敏感度在一定程度上体现了数据集的分布特征，因此只用局部敏感度来确定噪声强度大小，仍然容易造成隐私泄露。

拉普拉斯机制与指数机制
一般来说，拉普拉斯机制适合数值型结果的保护。在许多应用场合，查询结果都是非数值的，例如一个实体对象，这时应当使用的是指数机制，\Delta F与\Delta Q为差分隐私的敏感度。

---

**PSI：Private Set Intersection**隐私集合求交

[甘露](https://www.zhihu.com/people/lency)这个知乎上的作者不错

隐私集合求交（Private Set Intersection）是纵向联邦学习中的关键前置步骤，隐私集合求交就是双方通过加密计算，得到B、C两人这个集合，同时不暴露各自的原始集合的方法。

PSI是指，参与双方在不泄露任何额外信息的情况下，得到双方持有数据的交集。在这里，额外的信息指的是除了双方的数据交集以外的任何信息。

由隐私比较到不经意伪随机函数[隐私计算关键技术：隐私集合求交（PSI）原理介绍 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/367477035)（这里主要介绍的是两方PSI）
这样来看，我们就是使用不经意伪随机函数，来构建了一个隐私比较算法。

使用不经意伪随机函数构建隐私集合求交，将集合大小限制在O(n)的方法，原本的集合大小为O(n<sup>2</sup>)，经典的PSI的组件之一：**布谷鸟哈希（Cuckoo hashing）**，是一种哈希表的思想。大概的理解就是原本需要把每一个随机数种子对应每一个数据进行一遍计算伪随机函数计算然后进行传输大小为n<sup>2</sup>，现在可以使用一中Cuckoo hashing的策略去为对应的数据使用的k<sub>i</sub>建立索引，但是这个哈希函数有3个（3个桶），应该这里使用了随机的信息，保证了隐私？？

根据现在的隐私集合求交的算法，算法的速度取决于求交集的集合元素数量n，以及每个元素的大小l，也就是说**本篇论文解决了依赖于元素长度大小**这个问题？？

多方PSI，多方隐私集合求交可参考这个：[隐私计算关键技术：多方隐私集合求交（PSI）从原理到实现 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/431338434)

---

KK12通过shallow slices的电路评估，使用OT扩展有效地将其glue起来，从而遏制GESS方案秘密大小的高成本？？？

 [KK13]根据GMW用例提出了对短秘密size的改进？？？（本篇论文使用了KK13中的想法）

IKNP中的数据表示可以抽象地视为error-correcting code，好像可以这么理解吧，因为是对于列进行扩展的，如果有n行也就是n位选择bit，对其进行扩展应该可以最少扩展到log(n)位。KK13相对于IKNP使用了一种更好的编码表示（应该是这么理解），从而使1-2 OT变成了1-n OT（相同的成本），n可以高达256

对于OPRF的定义，一般来说receiver接收方都可以有多个输入，然后对PRF进行评估，但是本文之中只有进行单个OPRF的输入。
在本文之中通过修改IKNP和KK13协议的OT扩展协议的核心来构建本文的新的OPRF，本文的OPRF的内部结构比之前的work中的结构都更加接近OT

1-2 OT可以将输入域视为{1，2}，那么1-n OT则可以将随机消息的n个OT中的1个解释为输入域{1，…n}
在本篇论文之中主要就是对KK协议和IKNP协议的扩展，在与1-2的IKNP协议和KK OTs的相同cost下，完成n个随机消息选1个的OT协议，n为任意大。

**batched, related-key OPRF (BaRK-OPRF)**，本文提出的协议能够完成大量的OPRF实例使用一些相关的keys（密钥）

关于PSI的经典的论文：
Benny Pinkas, Thomas Schneider, Gil Segev, and Michael Zohner. Phasing: Private set intersection using permutation-based hashing. In Jaeyeon Jung and Thorsten Holz, editors, 24th
USENIX Security Symposium, USENIX Security 15, pages 515–530. USENIX Association, 2015.

Benny Pinkas, Thomas Schneider, and Michael Zohner. Faster private set intersection based
on OT extension. In Kevin Fu and Jaeyeon Jung, editors, 23rd USENIX Security Symposium,
USENIX Security 14, pages 797–812. USENIX Association, 2014.

本文中通过使用BaRK-OPRF这个组件对原来的PSSZ15  PSI协议之中的组件进行了替换，得到了很好的性能提升。对于中等长度也就是64-128bit的字符串以及相当大的集合求交之中取得了2.3-3.6倍的性能提升，对于大集合的128长度bit改进最为显著。

本文之中的BaRK-OPRF可以被视为一个OPRF协议也可以看作是基于IKNP的OT扩展变体。

KK13协议是什么啊：KK13协议是对IKNP扩展的一个推广，从1-2 OT扩展为了1-n OT，主要是通过编码理论进行扩展的。

KK协议应该是在列扩展的编码问题上进行了优化，允许使用大距离的纠错码(ECC)作为选择bit编码，也就是说KK在一定程度上推广泛化了IKNP协议。

接下来主要讲解KK13扩展中的内容（1-n OT）：
对于IKNP而言是简单的线性码也就是C(000000)或者是C(111111)，具体的C指的是什么可见：
[PSI系列（3） 组件 | OT Extension (KK13) - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/620120440)
信息位数k：信息位数可以理解为想要发送的真实数据的长度，而码长可以理解为实际传输过程中所用的数据长度。也就是可以用logq(K)来表示有规模多少的信息要发送。信息位数可以理解为想要发送的真实数据的长度，而码长可以理解为实际传输过程中所用的数据长度。

若所有非零码字C<sub>i</sub>的汉明重量都满足大于或等于\lamda，则计算所得的几个hash值是伪随机的。
![[Pasted image 20230417152549.png]]
一些经过扩展之后可以实现n选1的OT扩展协议。具体的讲解可见：
[Mike Rosulek系列课程笔记整理(完结): 不经意传输及其优化 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/381482519)


---


For a code consisting of n codewords, this allowed to do 1-out of-n OT with consuming a single row of the OT
extension matrix.（消耗OT扩展矩阵的单行去完成1-n的OT协议，对于具有n个码字的编码）？
解答：参考下图：
![[Pasted image 20230417153458.png]]
可见确实对于Alice方有数量为N的码字。即可完成一个1-n OT的离线阶段消息对（我的理解目前是这样的）（在协议的进行过程之中确实不需要对码字进行解码，好像PRC伪随机编码就可以）

本篇论文工作将编码理论发挥到了极致，消耗矩阵的一行完成1-poly的OT，3.5倍长于原IKNP）？？
其实对于这个问题就是使用伪随机编码可以完成∞选1的OT扩展。

Naor-Reingold PRF（这又是啥...？？？为啥用这个的OPRF的就需要进行幂运算了）

[CNs07]协议从唯一的盲签名方案（unique blind signature schemes：？？？）构建了一个OPRF。

---

接下来就是本篇论文主要探究了他们定义的OPRF在PSI之中的应用，主要和Pinkas等人的 [PSSZ15]协议关系最为密切，这个协议好像我也不知道是个啥...）？？ [PSSZ15]协议是 [PSSZ14]协议的优化变体。

基于OT的协议虽然运算很快但是communication cost并不低。如果要考虑很高的通信代价的话，推荐运用 Diffie-Hellman paradigm [HFH99]。在这些基于公钥的PSI协议之中每一方仅仅需要发送2n组元素，通信代价较低，但因为有模幂运算等复杂运算，所以性能很慢。

最直接的使用OPRF的协议需要receiver能够输入许多个input，然后通过PRF进行计算，但是本篇论文之中的自己设计的OPRF：whereas our OPRF allows only a single evaluation point for the receiver.？？

使用BaRK-OPRF，我们可以通过只消耗OT扩展矩阵的单行来实现关键字搜索。？？？这里又出现了KK12协议之中的SOT也就是通过k-bit selection strings去实现keyword search functionality功能，也就是 select the received secret via a string

OPRF还能够用于安全模式匹配（secure pattern matching），其中一方持有长文本T，另一方持有短模式字符串p。各方安全地得知了解T中所有p出现的位置。

OT的长度部分扩展方式的一种理解：例如，通过加密和发送两个m位的长字符串，并在短字符串上使用OT发送正确的解密密钥，也就是使用伪随机函数并发送随机数种子的过程抽象。

真离谱欸，IKNP中H用于消除线性的影响因素，可能这就是为啥H是一个correlation-robust hash function就可以的原因吧，不需要为严格的RO随机预言机模型。

KK13：令C为线性纠错码，l的维度，码字的长度为k，线性纠错码（ linear error correcting code ），根据编码理论，最小距离为k，对于receiver接收方来说，需要至少猜测秘密s之中的k位才行。

---

本篇论文中对编码进行了优化，提出了不需要保留很多纠错码的性质，使用PRC来进行编码。
- 我们的协议之中不需要解码，因此我们的编码不需要有效地解码。
- 事实上，即使汉明距离guarantee （证明）只是概率性的，也足够了？？？

伪随机编码能够使用伪随机函数以一种直接的方式进行实例化，but for now we simply point out that a random function with output length k = 4κ suffices to make near-collisions negligible in our applications.在实验过程之中精确地计算了参数，确认了C编码（函数）的输出长度。

本篇论文之中的也称为伪随机码，因为具有最小距离这个具有密码学含义的编码理论属性。remove the a-priori bound，也就是避免了纠错码接收方的选择字符串大小的先验约束。

本篇论文最主要的两个假设：一个就是C是一个伪随机码（可以视为伪随机函数），H满足correlation-robust，对于C使用PRF伪随机函数进行实例化，论文之中使用的是AES算法对C进行实例化，还使用现代处理器之中对于AES的加速（牛哇）

**汉明重量 Hamming Weight** 是一串符号中非零符号的个数。对于一个2进制表示为n bits的输入input，其非0位的个数为w，则w即为input的汉明重量。  
举例，input=10011010，其汉明重量w=4












