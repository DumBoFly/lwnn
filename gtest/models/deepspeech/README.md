
https://github.com/mozilla/DeepSpeech

```sh
cd lwnn/gtest/models/deepspeech
curl -LO https://github.com/mozilla/DeepSpeech/releases/download/v0.6.1/deepspeech-0.6.1-models.tar.gz
tar xvf deepspeech-0.6.1-models.tar.gz
curl -LO https://github.com/mozilla/DeepSpeech/releases/download/v0.6.1/audio-0.6.1.tar.gz
tar xvf audio-0.6.1.tar.gz
python mfcc.py audio/2830-3980-0043.wav
python mfcc.py audio/4507-16021-0012.wav
python mfcc.py audio/8455-210777-0068.wav

cd lwnn/gtest
python ../tools/tf2lwnn.py --dynamic_shape -i models/deepspeech/mfcc.pb -o dsmfcc --output_node output -s input 1,2 
python ../tools/tf2lwnn.py -i models/deepspeech/deepspeech-0.6.1-models/output_graph.pb -o deepspeech --input_node transpose --feeds models/deepspeech/feeds.json
```