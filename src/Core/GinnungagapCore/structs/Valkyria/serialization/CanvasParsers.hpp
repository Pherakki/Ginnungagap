#pragma once

#include <algorithm>
#include "CanvasDescriptors.hpp"
#include <exbip/framework/parsers/BaseVirtualStreamParser.hpp>


template<typename Implementation>
struct CanvasDescriptors 
  : public IOffset32Mixin<Implementation>
  , public IOffset64Mixin<Implementation>
  , public IOffset64sMixin<Implementation>
  , public IMemoizedString32Mixin<Implementation>
  , public IMemoizedString64Mixin<Implementation>
{};

template <typename Implementation>
struct CanvasDescriptorsLE
  : public exbip::StandardViewDescriptorsLE<Implementation>
  , public CanvasDescriptors<Implementation>
{};

template <typename Implementation>
struct CanvasDescriptorsBE
  : public exbip::StandardViewDescriptorsBE<Implementation>
  , public CanvasDescriptors<Implementation>
{};

/* -------- Reader -------- */

class CanvasReader : public exbip::IFileReaderBase<CanvasReader, CanvasDescriptorsLE, CanvasDescriptorsBE>
{
public:
   using exbip::IFileReaderBase<CanvasReader, CanvasDescriptorsLE, CanvasDescriptorsBE>::IFileReaderBase;
};

/* -------- Writer -------- */

class CanvasWriter : public exbip::IFileWriterBase<CanvasWriter, CanvasDescriptorsLE, CanvasDescriptorsBE>
{
public:
   using exbip::IFileWriterBase<CanvasWriter, CanvasDescriptorsLE, CanvasDescriptorsBE>::IFileWriterBase;
};

/* -------- Counter -------- */

class CanvasCounter : public exbip::ICounterBase<CanvasCounter, CanvasDescriptorsLE, CanvasDescriptorsBE>
{
public:
   using exbip::ICounterBase<CanvasCounter, CanvasDescriptorsLE, CanvasDescriptorsBE>::ICounterBase;
};

/* -------- OffsetCalculator -------- */

class CanvasOffsetCalculator : public exbip::IOffsetCalculatorBase<CanvasOffsetCalculator, CanvasDescriptorsLE, CanvasDescriptorsBE>
{
public:
   using exbip::IOffsetCalculatorBase<CanvasOffsetCalculator, CanvasDescriptorsLE, CanvasDescriptorsBE>::IOffsetCalculatorBase;
};

/* -------- POF1 -------- */
template <typename Descriptor, typename... Args>
concept hasPOF1Callable = requires(Descriptor t, Args... args) { { Descriptor::pof1(std::forward<Args>(args)...) }; };

template<class ParserImplementation, template<class> typename... Descriptors>
class IPOF1BaseView : public exbip::IStreamParserBaseView<ParserImplementation, IPOF1BaseView<ParserImplementation, Descriptors...>, Descriptors...>
{
public:
    using exbip::IStreamParserBaseView<ParserImplementation, IPOF1BaseView<ParserImplementation, Descriptors...>, Descriptors...>::IStreamParserBaseView;
public:
    template<class Descriptor, typename... Args>
    static decltype(auto) callDescriptor(Args&&... args)
    {
        if constexpr (hasPOF1Callable<Descriptor, Args...>)
            return Descriptor::pof1(std::forward<Args>(args)...);
        else
            return Descriptor::count(std::forward<Args>(args)...);
    }

    void register_offset(uint64_t offset)
    {
       this->held.register_offset(offset);
    }
};


template<class Implementation, template<class> typename LEMixins, template<class> typename BEMixins>
class IPOF1CalculatorBase : public exbip::IVirtualStreamParserBase<Implementation, IPOF1BaseView<Implementation, LEMixins>, IPOF1BaseView<Implementation, BEMixins>>
{
public:
    using exbip::IVirtualStreamParserBase<Implementation, IPOF1BaseView<Implementation, LEMixins>, IPOF1BaseView<Implementation, BEMixins>>::IVirtualStreamParserBase;
public:
   typedef uint64_t offset_t;

   std::vector<uint64_t> offsets;

   void register_offset(uint64_t offset)
   {
      if (offset > 0)
         this->offsets.push_back(this->tell());
   }
};

class POF1Calculator : public IPOF1CalculatorBase<POF1Calculator, CanvasDescriptorsLE, CanvasDescriptorsBE>
{
public:
   using IPOF1CalculatorBase<POF1Calculator, CanvasDescriptorsLE, CanvasDescriptorsBE>::IPOF1CalculatorBase;
};
