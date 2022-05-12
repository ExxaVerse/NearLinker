The security protocol is a key element in making sure that the wallet credentials are kept secure throughout the chain of operations. It allows to verify the security in to steps :
1. check that the protocol is secure. If not, it can be completed.
2. check that the implementation does what the protocol says. This makes the work of pen testers much easier, compared to digging in code blindly.

# Decritption of the protocol

Your game authority can deploy contracts to create tokens. The contract are signed by this authority to prove the authenticity of the tokens. These tokens are recognized by the game, which gives them a practical value, and they are sold to users.

The user has full control over his wallet. It is a NEAR wallet on the NEAR blockchain and is independent of your game. Therefore, his credentials are normally stored by him on his hardware. This information is sensitive and must therefore be encrypted on disk with a password, or even secured in a USB hardware wallet.

When the user launches the game, he needs to use his tokens. Some tokens grants him access to the game and he only needs to prove his identity, since everyone publicly knows what belongs to each wallet. Other tokens need to be gambled, so the user needs to call some contract functions to allow the game to transfer his tokens if he looses. For this, the user could theoritically call the required functions himself using his own trusty software. 
But in practice, he will trust the game client to do it, and give it some access data for his wallet. 

On the NEAR blockchain, this access data is a private key specific to each client software. Again, the user will trust the game client to generate the key pair, register the public key on his wallet, and store the private key safely.
For increased security, the key pair will be generated on the integration server and encrypted by its own private key, so that this key will be usable only through the integration server. This encrypted near private key will be the UE plugin's access data.

To register the generated public key, the game client will redirect the user to near.org/login page (same as NEAR CLI). 
If a malicious software could detect that the game is asking a key pair to the integration server, it could block it, make its own request, launch its own near.org/login with the key it has got, and launch a near.org/login page to trick the user into accepting it instead of the game's key. To prevent that, the game client should ask the user to not accept any key before it is told to do so.
If a malicious software could detect that the game is launching the near.org/login page to register a public key, it could launch its own page at the same time with its own key and trick the user into accepting it instead of the game's key. To prevent that, the game client should show the key to be added before launching the web page and ask the user to check that the near.org/login contains the same key.

The game client needs to keep its private key secure. So, the user will be asked for a password once per session. A hash of this password will be the key to encrypt and decrypt the private key stored on disk. 

When the password is provided, it is used immediately to load the wallet access data. Then the password and its hash are immediately zero erased from memory. At this point, the access data on the NearLinker plugin is useless without the integration server private key. The game will send it along each POST or PUT request to the integration server.

Now the sensitive data is the integration server private key. How and where is it stored ? The more practical for liability reasons is to run the integration server on the same computer as the game client, but it is also possible to run it elsewhere. If it runs on a different computer, then the game client computer is already secure, as well as the network connection to the integration server (https). If it runs on the same computer, the current user should not be able to write to any file used by the integration server nor read its private key. The user could ideally launch it (maybe with UAC), and the server process would have access to its own files. 

# About SSL certificates

In case the integration server runs on a server somewhere, it would have its own SSL certificate as a normal server.

In case the integration server runs on the same computer as the game, the game installer would:

- generate the SSL certificate chain (CA and server certificate) and the server private key
- install the game including the generated CA. Those files should be read-only and executable from the user. 
- install the integration server including the certificate chain and the private key. The user should not be able to read the private key, but the server should. The user should not be able to write any of those files. In other words, the server should be isolated from the user, just like if it was run on another computer, except that the user may start it.

Pen tester should check that the game client accepts the integration server certificate only if it is valid. See `badssl.com` for failure cases to check.

# Wallet management

Creating a wallet requires some money, so it cannot be done by the plugin alone. A wallet can be created by the user with his own tools (no need to trust a third party), or by the game seller when he sells the game (more convenient).

Then the plugin needs to get an access key to the wallet. It can generate a key pair and ask the user to register the new public key. Or this can be done when the user buys the game if the buying process creates a new wallet for him. In both cases, the newly generated private key should be encrypted as soon as possible with a password that only the user knows.

