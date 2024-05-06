#include <iostream>
#include <streambuf>

// Custom stream buffer that can be toggled on and off
class ToggleableBuffer : public std::streambuf {
public:
    ToggleableBuffer(std::ostream &output_stream)
        : output_stream(output_stream), enabled(true) {}

    void toggle(bool state) {
        enabled = state;
    }

protected:
    virtual int overflow(int c) override {
        if (enabled && c != EOF) {
            output_stream.put(c);
        }
        return c;
    }

private:
    std::ostream &output_stream;
    bool enabled;
};

// Custom output stream that uses the toggleable buffer
class ToggleableStream : public std::ostream {
public:
    ToggleableStream(std::ostream &output_stream)
        : std::ostream(&buffer), buffer(output_stream) {}

    void toggle(bool state) {
        buffer.toggle(state);
    }

private:
    ToggleableBuffer buffer;
};
