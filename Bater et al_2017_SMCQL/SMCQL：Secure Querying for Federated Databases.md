
本篇论文主要提出的为PDN的私有数据网络（应该就是多方的federated database进行安全的查询）

Instead, the user submits a query to an honest broker that plans and coordinates its execution over multiple private databases using secure multiparty computation (SMC).（用户将查询提交给一个诚实的代理，该代理使用安全多方计算(SMC)在多个私有数据库上计划和协调查询的执行。）

each database’s query execution is oblivious：这里的oblivious是对于其他用户不可见的意思？？

This system translates SQL statements into SMC primitives to compute query results over the union of
its source databases without revealing sensitive information about individual tuples to peer data providers or the honest broker. （对于对等的数据提供方与代理方都不会揭露敏感数据）

Here, a user submits their query to the federation’s honest broker, a neutral third party that plans and orchestrates its execution over two or more private data providers.（一个用户将查询提交给honest broker，一个中立的第三方）

 produces a share of the query output.（？？？）（我的理解是对于每个本地数据源，计算出来query output的一个分片或者share共享的一部分，然后在honest broker重新assemble）
 
This schema is annotated with the level of protection required for each of its attributes. （这个模式标注了每个属性所需的保护级别。）

cross product of its inputs：这里是叉积的意思么，也就是需要使用加密的NULL进行填补

we present a query planner and executor for two mutually distrustful parties.（不信任的两方，提出了一个查询规划器和一个查询执行器）

Our approach keeps data in the hands its originators and uses SMC to conceal the query’s computation rather than the data itself.（我们的方法将数据保存在其发起者手中，并使用SMC来隐藏查询的计算而不是数据本身。）

我们在这项工作中的主要贡献是:
• 对联邦数据库的一种新颖的泛化，用于对互不信任的数据提供者进行查询。
• 自动将SQL转换为SMC原语的代码生成器。
• 基于启发式的PDN查询执行优化器
• 在真实医疗数据上对该系统进行深入评估。

ORAM：Oblivious RAM
ORAM(Oblivious Random Access Machine，茫然随机访问机)是一种可以用来完全隐藏IO操作的数据访问模式的[加密](https://so.csdn.net/so/search?q=%E5%8A%A0%E5%AF%86&spm=1001.2101.3001.7020)方案。访问模式是指IO操作访问文件的顺序、访问文件的频率、读写顺序等，当用户把数据存储在不可信的第三方时，即使数据是加密的，第三方仍能通过收集用户访问模式信息推断出用户隐私，在ORAM方案中，若两次访问序列长度相同，则其访问模式是相同的，使得第三方无法通过访问模式获取用户隐私。简单来说，ORAM将用户的一个文件访问请求转换成多个文件访问请求，从而模糊化用户访问文件的概率、模式等信息。目前ORAM研究较多的领域是云存储安全。
参考链接：[(125条消息) ORAM简介_Joey Liao的博客-CSDN博客](https://blog.csdn.net/c630843901/article/details/120655057)

In our framework, the only information available to the data providers is the number of tuples provided by each party.（这里的意思是在本文提出的框架之中唯一可获取的available的就是元组的数量）

感觉本文的整体流程为：首先将SQL进行转化转化为SMC原语，然后通过混淆电路进行计算每个operator中间通过ORAM将每次计算的结果连接起来。

在本文之中主要对三种主流的医院数据研究所需要的SQL语句进行探究，主要分为三个研究优化方向：
1. 我们探索了在分布式查询评估中最小化SMC使用的实用技术。
2. 我们使用这个查询来检查细粒度数据分区(按患者ID)如何提高smcql的性能。
3. 这个查询测试smcql优化器为复杂的操作符序列创建高性能查询计划的能力。

The honest broker examines this tree, confirming that it is runnable within the PDN’s security policy.（这个PDN中的诚实代理会在拿到SQL语句解析得到的解析树之后，确认这个查询策略在PDN的安全策略内可行）

For each relational algebra operator, the code generator looks up a template for it and populates this outline  with query-specific information including the width of its tuples in bits and filter predicates.（对于每个关系代数运算符，code generator会提供一个模板，然后根据元组的bit长度和谓词过滤进行特定的填充）

ObliVM，ObliC（这些应该是用的比较多的框架）

ObliVM：This framework compiles its code in two steps.
1. 首先，它将代码转换为一组逻辑门和ORAM访问。
2. 然后，在执行时，它在快速地产生乱码电路以防止重放攻击。

每个安全操作符都以模板或参数化的c风格程序开始，用于操作符的执行逻辑。模板具有用于过滤器谓词、输入元组宽度以及根据需要投影其输出的变量。

Section3 是整体流程的讲解是比较重要的。

本文其实叙述的顺序和Section3中提到的流程是相反的，本文的叙述顺序是先从SMC对操作符的翻译和编译这一段先开始讲的。

本文之中的Section 5就是这句话所揭示的 we first introduce a security model with which we identify query evaluation over sensitive data. 也就是说，我们不可能对所有的SQL语句进行SMC计算，我们只能最小化将其分割扩展等，所以我们首先需要一个security model将其敏感数据流进行确认。

In each operator, we examine the provenance of its output columns and determine the protection level needed for each one by taking the maximum security policy of its source attributes.：在每个操作符中，我们检查其输出列的来源，并通过采用其源属性的最大安全策略来确定每个列所需的保护级别。

这个the secure type system好像真的很像编译器里面的type system，类型系统，grammar就是编译器里面的上下文无关语法啥的应该是。

sliced：这个我好像基本懂了。

但是这个切片的安全性证明还是有点儿懵

拆分运算的效果：将执行划分为离散的阶段，感觉切片这里是根据slice key进行分组进行切片操作，而拆分运算则是SQL语句下推，拆分为各方计算下推到本地进行计算。