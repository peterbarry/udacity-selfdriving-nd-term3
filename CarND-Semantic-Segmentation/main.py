import os.path
import tensorflow as tf
import helper
import warnings
from distutils.version import LooseVersion
import project_tests as tests


# Some test setup variables.

run_test = True
use_checkpoint = False


# Check TensorFlow Version
assert LooseVersion(tf.__version__) >= LooseVersion('1.0'), 'Please use TensorFlow version 1.0 or newer.  You are using {}'.format(tf.__version__)
print('TensorFlow Version: {}'.format(tf.__version__))

# Check for a GPU
if not tf.test.gpu_device_name():
    warnings.warn('No GPU found. Please use a GPU to train your neural network.')
else:
    print('Default GPU Device: {}'.format(tf.test.gpu_device_name()))


def load_vgg(sess, vgg_path):
    """
    Load Pretrained VGG Model into TensorFlow.
    :param sess: TensorFlow Session
    :param vgg_path: Path to vgg folder, containing "variables/" and "saved_model.pb"
    :return: Tuple of Tensors from VGG model (image_input, keep_prob, layer3_out, layer4_out, layer7_out)
    """
    # TODO: Implement function
    #   Use tf.saved_model.loader.load to load the model and weights
    vgg_tag = 'vgg16'
    vgg_input_tensor_name = 'image_input:0'
    vgg_keep_prob_tensor_name = 'keep_prob:0'
    vgg_layer3_out_tensor_name = 'layer3_out:0'
    vgg_layer4_out_tensor_name = 'layer4_out:0'
    vgg_layer7_out_tensor_name = 'layer7_out:0'

    tf.saved_model.loader.load(sess, [vgg_tag], vgg_path)

    # Model is loaded, but need to get the tensors from the graph.
    graph = tf.get_default_graph()
    input1 = graph.get_tensor_by_name(vgg_input_tensor_name)
    keep = graph.get_tensor_by_name(vgg_keep_prob_tensor_name)
    l3 = graph.get_tensor_by_name(vgg_layer3_out_tensor_name)
    l4 = graph.get_tensor_by_name(vgg_layer4_out_tensor_name)
    l7 = graph.get_tensor_by_name(vgg_layer7_out_tensor_name)

    print("Summary  of Loaded network")
    tf.summary.image('image_input', input1)
    tf.summary.histogram('layer3_out', l3)
    tf.summary.histogram('layer4_out', l4)
    tf.summary.histogram('layer7_out', l7)

    return input1, keep, l3, l4, l7

if ( run_test == True):
    print("About to test load vgg\n")
    tests.test_load_vgg(load_vgg, tf)


def layers(vgg_layer3_out, vgg_layer4_out, vgg_layer7_out, num_classes):
    """
    Create the layers for a fully convolutional network.  Build skip-layers using the vgg layers.
    :param vgg_layer7_out: TF Tensor for VGG Layer 3 output
    :param vgg_layer4_out: TF Tensor for VGG Layer 4 output
    :param vgg_layer3_out: TF Tensor for VGG Layer 7 output
    :param num_classes: Number of classes to classify
    :return: The Tensor for the last layer of output
    """
    # This is the build of the FCN.
    #
    #
    # Get a convolution of all layers we are going to use. in decoder.

    conv_1x1_layer7 = tf.layers.conv2d(vgg_layer7_out,
                                num_classes,
                                kernel_size=(1, 1),
                                strides=(1, 1), padding='same',
                                kernel_regularizer=tf.contrib.layers.l2_regularizer(1e-3))
    conv_1x1_layer4 = tf.layers.conv2d(vgg_layer4_out,
                                num_classes,
                                kernel_size=(1, 1),
                                strides=(1, 1), padding='same',
                                kernel_regularizer=tf.contrib.layers.l2_regularizer(1e-3))
    conv_1x1_layer3 = tf.layers.conv2d(vgg_layer3_out,
                                num_classes,
                                kernel_size=(1, 1),
                                strides=(1, 1), padding='same',
                                kernel_regularizer=tf.contrib.layers.l2_regularizer(1e-3))

    # Upsample convoled layer 7 by 2.

    out_7 = tf.layers.conv2d_transpose(conv_1x1_layer7,
                                    num_classes,
                                    4,
                                    (2,2) ,
                                    padding='same',
                                    kernel_initializer=tf.random_normal_initializer(stddev=0.01),
                                    kernel_regularizer=tf.contrib.layers.l2_regularizer(1e-3))

    # Add upscaled  layer 7 with convolved layer 4
    out_4_skip = tf.add(out_7, conv_1x1_layer4)

    # Scale up by 2.
    out_4 = tf.layers.conv2d_transpose(out_4_skip, num_classes, 4, (2,2),
            padding='same',
            kernel_initializer=tf.random_normal_initializer(stddev=0.01),
            kernel_regularizer=tf.contrib.layers.l2_regularizer(1e-3)
            )

    # Add convolved - skip  layer 3
    out_4_skiped = tf.add(out_4,conv_1x1_layer3 )

    # Add upscaled  x8 to origional image size.
    output = tf.layers.conv2d_transpose(out_4_skiped, num_classes, 16, (8,8),
            padding='same',
            kernel_initializer=tf.random_normal_initializer(stddev=0.01),
            kernel_regularizer=tf.contrib.layers.l2_regularizer(1e-3)
            )



    return output

if ( run_test == True):
    print("About to test_layers - !!\n")
    tests.test_layers(layers)


def optimize(nn_last_layer, correct_label, learning_rate, num_classes):
    """
    Build the TensorFLow loss and optimizer operations.
    :param nn_last_layer: TF Tensor of the last layer in the neural network
    :param correct_label: TF Placeholder for the correct label image
    :param learning_rate: TF Placeholder for the learning rate
    :param num_classes: Number of classes to classify
    :return: Tuple of (logits, train_op, cross_entropy_loss)
    """
    # TODO: Implement function
    logits = tf.reshape(nn_last_layer, (-1, num_classes))

    _entropy_loss = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits= logits, labels= correct_label))

    _optimizer = tf.train.AdamOptimizer(learning_rate= learning_rate)
    train_op = _optimizer.minimize(_entropy_loss)


    return logits, train_op , _entropy_loss

if ( run_test == True):
    print("About to test_optimize\n")
    tests.test_optimize(optimize)


def train_nn(sess, epochs, batch_size, get_batches_fn, train_op, cross_entropy_loss, input_image,
             correct_label, keep_prob, learning_rate):
    """
    Train neural network and print out the loss during training.
    :param sess: TF Session
    :param epochs: Number of epochs
    :param batch_size: Batch size
    :param get_batches_fn: Function to get batches of training data.  Call using get_batches_fn(batch_size)
    :param train_op: TF Operation to train the neural network
    :param cross_entropy_loss: TF Tensor for the amount of loss
    :param input_image: TF Placeholder for input images
    :param correct_label: TF Placeholder for label images
    :param keep_prob: TF Placeholder for dropout keep probability
    :param learning_rate: TF Placeholder for learning rate
    """
    # TODO: Implement function
    print("Starting to Train")

    for i in range(epochs):
        print("Epcoh:{:d}".format(i))
        for image, label in get_batches_fn(batch_size):
            ignore_, loss_val = sess.run([train_op, cross_entropy_loss],
                               feed_dict={input_image: image,
                                correct_label: label,
                                keep_prob: 0.5, learning_rate: 0.0010})

            print("Loss: = {:.4f}".format(loss_val))
    pass

if ( run_test == True):
    print("About to test_train_nn\n")
    tests.test_train_nn(train_nn)


def run():
    num_classes = 3
    image_shape = (160, 576)
    data_dir = './data'
    runs_dir = './runs'
    print("About to test_for_kitti_dataset\n")

    tests.test_for_kitti_dataset(data_dir)

    # Download pretrained vgg model
    helper.maybe_download_pretrained_vgg(data_dir)

    # OPTIONAL: Train and Inference on the cityscapes dataset instead of the Kitti dataset.
    # You'll need a GPU with at least 10 teraFLOPS to train on.
    #  https://www.cityscapes-dataset.com/


    with tf.Session() as sess:
        # Path to vgg model
        vgg_path = os.path.join(data_dir, 'vgg')
        # Create function to get batches
        get_batches_fn = helper.gen_batch_function(os.path.join(data_dir, 'data_road/training'), image_shape)

        # OPTIONAL: Augment Images for better results
        #  https://datascience.stackexchange.com/questions/5224/how-to-prepare-augment-images-for-neural-network

        num_epochs = 100
        batch_size = 10

        # TF placeholders
        correct_label = tf.placeholder(tf.int32, [None, None, None, num_classes], name='correct_label')
        learning_rate = tf.placeholder(tf.float32, name='learning_rate')

        # TODO: Build NN using load_vgg, layers, and optimize function
        input_image, keep_prob, layer3_out, layer4_out, layer7_out = load_vgg(sess, vgg_path)

        output_layer = layers(layer3_out, layer4_out, layer7_out, num_classes)

        logits, train_op, cross_entropy_loss = optimize(output_layer, correct_label, learning_rate, num_classes)

        # 'Saver' op to save and restore all the variables
        model_saver = tf.train.Saver()


        if (use_checkpoint == False):
        # 'init vars.

            sess.run(tf.global_variables_initializer())

            # TODO: Train NN using the train_nn function
            train_nn(sess, num_epochs,
                       batch_size,
                       get_batches_fn,
                       train_op,
                       cross_entropy_loss,
                       input_image,
                       correct_label,
                       keep_prob,
                       learning_rate)

            save_path = model_saver.save(sess, "./model/model")
            print("Model saved in file: %s" % save_path)
        else:
            sess.run(tf.global_variables_initializer())

            saver = tf.train.Saver()
            print("Loading saved model for interence\n")
            saver.restore(sess, './model')
            print("Loading complete\n")
            helper.save_inference_samples(runs_dir, data_dir, sess, image_shape, logits, keep_prob, input_image)

        # OPTIONAL: Apply the trained model to a video


if __name__ == '__main__':
    run()
