The security protocol is a key element in making sure that the wallet credentials are kept secure throughout the chain of operations. It allows to verify the security in to steps :
1. check that the protocol is secure. If not, it can be completed.
2. check that the implementation does what the protocol says. This makes the work of pen testers much easier, compared to digging in code blindly.

# Decritption of the protocol

Your game authority can deploy contracts to create tokens. The contract are signed by this authority to prove the authenticity of the tokens. These tokens are recognized by the game, which gives them a practical value, and they are sold to users.

The user has full control over his wallet. It is a NEAR wallet on the NEAR blockchain and is independent of your game. Therefore, his credentials are normally stored by him on his hardware. This information is sensitive and must therefore be encrypted on disk with a password, or even secured in a USB hardware wallet.

When the user launches the game, he needs to use his tokens. Some tokens grants him access to the game and he only needs to prove his identity, since everyone publicly knows what belongs to each wallet. Other tokens need to be gambled, so the user needs to call some contract functions to allow the game to transfer his tokens if he looses. For this, the user could theoritically call the required functions himself using his own trusty software. 
But in practice, he will trust the game client to do it, and give it some access data for his wallet. 

The game client needs to keep this valuable information secure. For practical reasons, the users will be asked for a password once (or a hardware wallet, do we support this ?). A hash of this password is the key to decrypt the access data stored on disk. It is used immediately to load the access data, which is immediately encrypted again using the integration server public key. The password, its hash, and the plain access data is then immediately zero erased from memory. At this point, the access data is useless without the integration server private key. The game will send it along each POST or PUT request to the integration server.

Now the sensitive data is the integration server private key. How and where is it stored ? The more practical for liability reasons is to run the integration server on the same computer as the game client, but it is also possible to run it elsewhere. If it runs on a different computer, then the game client computer is already secure, as well as the network connection to the integration server (https). If it runs on the same computer, the current user should not be able to write to any file used by the integration server nor read its private key. The user could ideally launch it (maybe with UAC), and the server process would have access to its own files. 

# About SSL certificates

In case the integration server runs on a server somewhere, it would have its own SSL certificate as a normal server.

In case the integration server runs on the same computer as the game, the game installer would generate the SSL certificate and its private key, install the game in user space including the SSL certificate, install the integration server in admin space including the corresponding private key.

