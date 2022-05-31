!["Near Linker Logo](docs/images/NearLinker-Logo-Light.png#gh-dark-mode-only)
!["Near Linker Logo](docs/images/NearLinker-Logo-Dark.png#gh-light-mode-only)

<p align="center">
  <a aria-label="License" href="https://github.com/ExxaVerse/NearLinker/blob/main/LICENSE">
    <img alt="" src="https://img.shields.io/npm/l/next.svg?style=for-the-badge&labelColor=000000">
  </a>
  <a aria-label="Join the community on GitHub" href="https://github.com/ExxaVerse/NearLinker/discussions">
    <img alt="" src="https://img.shields.io/badge/Checkout%20the%20discussion-blue.svg?style=for-the-badge&labelColor=000000&logoWidth=20">
  </a>
</p>

# What is NearLinker?

NearLinker is an open source project that will allow developers to integrate [NEAR](https://near.org/) blockchain in games developed with Unreal Engine.

# Overview

The project is composed of two main components: the integration server and the Unreal Engine plugin. The integration server is created in [Node.js](https://nodejs.org/en/) and its main purpose is creating a communication channel with the NEAR blockchain. It will leverage [next-api-js](https://docs.near.org/docs/api/javascript-library) to establish communication with NEAR, while exposing REST API endpoints to be accessed from the outside. The Unreal Engine plugin will be configured by the developer to communicate directly with the NL server. It will be plugged inside Unreal Engine projects and will allow the features discussed below.

Smart contracts will be created and pushed separately on the NEAR blockchain. Implementation for smart contracts is done in Rust.

NearLinker will be featured in developing [Exxaverse](https://exxaverse.com/), a gladiator-themed play-to-earn game.

# Features

The following features will be implemented with the delivery of the project's MVP:

- Allowing users to view and transfer their fungible and non-fungible assets
- Real-time interaction with smart contracts
- Data encryption methods on both UE4 and integration server sides

More features will be disclosed closer to the full-launch.

# Architecture

![NearLinker Architecture](docs/images/NearLinker-Architecture-Dark.png#gh-light-mode-only)
![NearLinker Architecture](docs/images/NearLinker-Architecture-Light.png#gh-dark-mode-only)

Architecture breakdown:

- `NEAR blockchain` - the main data storage
- `RUST Smart Contracts` - smart contracts are written in Rust and deployed directly on the blockchain
- `NodeJS Integration Server` - communicates with the NEAR blockchain through specific libraries
- `UE NearLinker plugin` - provides Blueprints to communicate with the Integration Server
- `UE Games` - games developed in Unreal Engine that use the NearLinker plugin to connect to the blockchain

# Demo project

A demo project is located in `nl-ue4-example-project/`. To use it, you need to 
1. Clone the [NearLinker_UE_plugin](https://github.com/ExxaVerse/NearLinker_UE_plugin) by running `git submodule update --init`.
2. Follow the instructions in `nl-ue4-example-project/Plugins/NearLinker/README.md`.

The project is blank with only one simple actor in it which only spawns the BP_NearMenu from the plugin. This menu is a simple UI, showcasing all functionalities. This BP_NearMenu is a good place to see how to use the plugin in Blueprints.

In each project where you want to use the plugin, you need to clone it in the `<Project>/Plugins/` directory, and follow the instructions in `nl-ue4-example-project/Plugins/NearLinker/README.md`.

# FAQ & Contact
