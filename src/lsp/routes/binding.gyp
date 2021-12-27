{
  "targets": [
    {
      "target_name": "km2lsp-node",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [
        "../greeting.cpp",
        "../index.cpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
      "libraries": [
            "-km2", "-L/home/admin/MyLib/Library/binaries/linux/Release"
        ],
    }
  ]
}
