// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

import "@openzeppelin/contracts/token/ERC1155/ERC1155.sol";

contract Inventory is ERC1155 {
    uint256 public constant COOKIE = 0;
    uint256 public constant APPLE = 1;
    uint256 public constant CHEESE = 2;
    uint256 public constant POTATO = 3;

    constructor() ERC1155("https://nftstorage.link/ipfs/bafybeif2uvhr6ihxkg3cmpa4rbyqfbfmoop25tg2klt5bflyeydaaue6ly/{id}.json") {}

    function mint(uint256 id) public {
        require(id < 4, "invalid item id");
        _mint(msg.sender, id, 1, "");
    }
}
